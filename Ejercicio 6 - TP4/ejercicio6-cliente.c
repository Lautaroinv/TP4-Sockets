
/* 
Ejercicio 6
Objetivo: programar con sockets de datagramas y compensar la falta de confiabilidad de UDP desde la aplicación.

ej06_udp_cliente.c (./ej06_udp_cliente <host> <puerto> [conectado]):
1.​ Envía tres datagramas de tamaños distintos (por ejemplo, 1, 18 y 1000 bytes) y
verifica que cada respuesta tenga exactamente el tamaño enviado.
2.​ Espera cada respuesta como máximo 2 segundos, usando la opción
SO_RCVTIMEO.
3.​ Si no llega respuesta, reintentar hasta 3 veces antes de abandonar ese mensaje,
informando cada intento.
4.​ Sin el argumento conectado, usa sendto() y recvfrom(). Con el argumento, usa
connect() y luego send() y recv().

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>

#define MAX_INTENTOS 3
#define TIMEOUT_SEGUNDOS 2

int main(int argc, char *argv[]) {

    printf("\n");

    /* Comprobar los argumentos */
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Uso: %s <host> <servicio> [conectado]\n", argv[0]);
        return 1;
    }

    int conectado = 0;

    if (argc == 4) {
        if (strcmp(argv[3], "conectado") != 0) {
            fprintf(stderr, "El argumento opcional debe ser 'conectado'\n");
            return 1;
        }
        conectado = 1;
    }

    /* Resolver el destino */
    struct addrinfo hints = {0};
    struct addrinfo *lista;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int error = getaddrinfo(argv[1], argv[2], &hints, &lista);

    if (error != 0) {
        fprintf(stderr, "Error en getaddrinfo: %s\n", gai_strerror(error));
        return 1;
    }

    /* Crear el socket UDP */
    int fd = socket(lista->ai_family, lista->ai_socktype,
                    lista->ai_protocol);

    if (fd < 0) {
        perror("socket");
        freeaddrinfo(lista);
        return 1;
    }

    /* Configurar el tiempo máximo de espera */
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SEGUNDOS;
    timeout.tv_usec = 0;

    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO,
                   &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt");
        close(fd);
        freeaddrinfo(lista);
        return 1;
    }

    /* Conectar el socket si se solicita el modo conectado */
    if (conectado) {
        if (connect(fd, lista->ai_addr, lista->ai_addrlen) < 0) {
            perror("connect");
            close(fd);
            freeaddrinfo(lista);
            return 1;
        }

        printf("Modo UDP conectado.\n");
    } else {
        printf("Modo UDP no conectado.\n");
    }

    /* Preparar los datagramas de distintos tamaños */
    size_t tamanos[] = {1, 18, 1000};
    char buffer[1000];
    char respuesta[1000];

    memset(buffer, 'A', sizeof(buffer));

    for (int i = 0; i < 3; i++) {

        size_t cantidad = tamanos[i];
        int correcto = 0;

        printf("\nDatagrama de %zu bytes:\n", cantidad);

        /* Enviar y esperar respuesta, con hasta 3 intentos  */
        for (int intento = 1; intento <= MAX_INTENTOS; intento++) {

            printf("Intento %d de %d...\n", intento, MAX_INTENTOS);

            ssize_t enviados;

            if (conectado) {
                enviados = send(fd, buffer, cantidad, 0);
            } else {
                enviados = sendto(fd, buffer, cantidad, 0,
                                  lista->ai_addr, lista->ai_addrlen);
            }

            if (enviados < 0) {
                perror("Error al enviar");
                continue;
            }

            ssize_t recibidos;

            if (conectado) {
                recibidos = recv(fd, respuesta, sizeof(respuesta), 0);
            } else {
                recibidos = recvfrom(fd, respuesta, sizeof(respuesta),
                                     0, NULL, NULL);
            }

            if (recibidos < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    printf("Tiempo de espera agotado.\n");
                } else {
                    perror("Error al recibir");
                }
                continue;
            }

            if ((size_t)recibidos == cantidad) {
                printf("Respuesta correcta: %zd bytes recibidos.\n",
                       recibidos);
                correcto = 1;
                break;
            }

            printf("Respuesta incorrecta: se esperaban %zu bytes "
                   "y se recibieron %zd.\n", cantidad, recibidos);
        }

        if (!correcto) {
            printf("No se obtuvo una respuesta válida tras %d intentos.\n",
                   MAX_INTENTOS);
        }
    }

    /* Liberar recursos */
    close(fd);
    freeaddrinfo(lista);

    printf("\n");

    return 0;
}

