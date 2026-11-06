
/* 
Ejercicio 6
Objetivo: programar con sockets de datagramas y compensar la falta de confiabilidad de UDP desde la aplicación.

Consigna: escribir dos programas.
ej06_udp_servidor.c (./ej06_udp_servidor <puerto>): recibe datagramas con recvfrom(), muestra la dirección de
origen y el tamaño de cada uno, y los devuelve al remitente con sendto().

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {

    printf("\n");

    /* Comprobar los argumentos */
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <puerto>\n", argv[0]);
        return 1;
    }

    int puerto = atoi(argv[1]);

    if (puerto < 1 || puerto > 65535) {
        fprintf(stderr, "Puerto invalido\n");
        return 1;
    }

    /* Crear un socket UDP */
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (fd < 0) {
        perror("socket");
        return 1;
    }

    /* Configurar la dirección del servidor */
    struct sockaddr_in dir = {0};
    dir.sin_family = AF_INET;
    dir.sin_addr.s_addr = htonl(INADDR_ANY);
    dir.sin_port = htons(puerto);

    /* Asociar el socket al puerto */
    if (bind(fd, (struct sockaddr *)&dir, sizeof(dir)) < 0) {
        perror("bind");
        close(fd);
        return 1;
    }

    printf("Servidor UDP escuchando en el puerto %d...\n", puerto);

    /* Recibir datagramas y devolverlos al remitente */
    while (1) {

        char buffer[2048];

        struct sockaddr_in origen;
        socklen_t tam_origen = sizeof(origen);

        ssize_t recibidos = recvfrom(
            fd, buffer, sizeof(buffer), 0,
            (struct sockaddr *)&origen, &tam_origen
        );

        if (recibidos < 0) {
            if (errno == EINTR)
                continue;

            perror("recvfrom");
            break;
        }

        /* Mostrar la dirección de origen y el tamaño */
        char ip[INET_ADDRSTRLEN];

        if (inet_ntop(AF_INET, &origen.sin_addr, ip, sizeof(ip)) == NULL) {
            perror("inet_ntop");
            continue;
        }

        printf("Datagrama recibido de %s:%u (%zd bytes)\n",
               ip, ntohs(origen.sin_port), recibidos);

        /* Devolver el datagrama al remitente */
        ssize_t enviados = sendto(
            fd, buffer, recibidos, 0,
            (struct sockaddr *)&origen, tam_origen
        );

        if (enviados < 0) {
            perror("sendto");
        } else if (enviados != recibidos) {
            fprintf(stderr, "No se enviaron todos los bytes\n");
        }
    }

    close(fd);

    printf("\n");

    return 0;
}

