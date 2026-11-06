/* 
Ejercicio 5
Objetivo: implementar un cliente y un servidor TCP completos y observar el estado
TIME_WAIT del cierre de conexión.

ej05_eco_cliente.c (./ej05_eco_cliente <host> <servicio>):
1.​ Resuelve el destino con getaddrinfo() y prueba cada dirección de la lista hasta que
una conexión tenga éxito, mostrando cada intento y su resultado.
2.​ Lee líneas del teclado, las envía al servidor y muestra la respuesta completa.
3.​ Termina cuando se ingresa fin de entrada (Ctrl+D).

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

/* Garantiza el envío de todos los bytes */
static int enviar_todo(int fd, const char *datos, size_t cantidad) {
    size_t enviados = 0;

    while (enviados < cantidad) {
        ssize_t n = send(fd, datos + enviados, cantidad - enviados, 0);

        if (n <= 0)
            return -1;

        enviados += (size_t)n;
    }

    return 0;
}

/* Recibe todos los bytes esperados */
static int recibir_todo(int fd, char *datos, size_t cantidad) {
    size_t recibidos = 0;

    while (recibidos < cantidad) {
        ssize_t n = recv(fd, datos + recibidos, cantidad - recibidos, 0);

        if (n <= 0)
            return -1;

        recibidos += (size_t)n;
    }

    return 0;
}

int main(int argc, char *argv[]) {

    printf("\n");

    /* Verificar los argumentos */
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <host> <servicio>\n", argv[0]);
        return 1;
    }

    /* Configurar los parámetros de resolución */
    struct addrinfo hints, *lista;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    /* Resolver el destino */
    int err = getaddrinfo(argv[1], argv[2], &hints, &lista);

    if (err != 0) {
        fprintf(stderr, "Error en getaddrinfo: %s\n", gai_strerror(err));
        return 1;
    }

    /* Probar cada dirección hasta conseguir una conexión */
    int conexion = -1;

    for (struct addrinfo *p = lista; p != NULL; p = p->ai_next) {

        char ip[NI_MAXHOST];

        getnameinfo(p->ai_addr, p->ai_addrlen,
                    ip, sizeof ip, NULL, 0, NI_NUMERICHOST);

        printf("Intentando conectar con %s...\n", ip);

        int fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if (fd < 0) {
            perror("Error al crear el socket");
            continue;
        }

        if (connect(fd, p->ai_addr, p->ai_addrlen) == 0) {
            conexion = fd;
            printf("Conexión establecida con %s\n", ip);
            break;
        }

        perror("Error en connect");
        close(fd);
    }

    freeaddrinfo(lista);

    if (conexion < 0) {
        fprintf(stderr, "No se pudo conectar con el servidor.\n");
        return 1;
    }

    /* Leer líneas del teclado y enviarlas al servidor */
    char buffer[1024];

    while (fgets(buffer, sizeof buffer, stdin) != NULL) {

        size_t cantidad = strlen(buffer);

        if (enviar_todo(conexion, buffer, cantidad) < 0) {
            perror("Error al enviar datos");
            break;
        }

        /* Recibir la respuesta completa del servidor */
        if (recibir_todo(conexion, buffer, cantidad) < 0) {
            fprintf(stderr, "Error al recibir la respuesta completa.\n");
            break;
        }

        buffer[cantidad] = '\0';
        printf("Respuesta: %s", buffer);
    }

    /* Cerrar la conexión */
    close(conexion);

    printf("\n");

    return 0;
}
