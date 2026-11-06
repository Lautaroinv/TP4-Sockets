/* 
Ejercicio 5
Objetivo: implementar un cliente y un servidor TCP completos y observar el estado
TIME_WAIT del cierre de conexión.

ej05_eco_servidor.c (./ej05_eco_servidor <puerto>):
1.​ Atiende a los clientes de a uno por vez.
2.​ Devuelve al cliente todo lo que recibe, hasta que este cierre la conexión.
3.​ Muestra cuándo se conecta y cuándo se desconecta cada cliente.
4.​ Debe garantizar que se envían todos los bytes recibidos, aunque send() envíe menos de lo pedido.
5.​ Debe aceptar un segundo argumento opcional, noreuse, que desactiva la opción
SO_REUSEADDR.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
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

int main(int argc, char *argv[]) {

    printf("\n");

    /* Validar los argumentos */
    if (argc < 2 || argc > 3 ||
        (argc == 3 && strcmp(argv[2], "noreuse") != 0)) {
        fprintf(stderr, "Uso: %s <puerto> [noreuse]\n", argv[0]);
        return 1;
    }

    int puerto = atoi(argv[1]);
    int reutilizar = (argc == 2);

    /* Crear el socket TCP */
    int servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor < 0) {
        perror("Error al crear el socket");
        return 1;
    }

    /* Configurar SO_REUSEADDR si corresponde */
    if (reutilizar) {
        int opcion = 1;

        if (setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR,
                       &opcion, sizeof opcion) < 0) {
            perror("Error en setsockopt");
            close(servidor);
            return 1;
        }
    }

    /* Configurar la dirección y el puerto */
    struct sockaddr_in dir;
    memset(&dir, 0, sizeof dir);

    dir.sin_family = AF_INET;
    dir.sin_addr.s_addr = htonl(INADDR_ANY);
    dir.sin_port = htons(puerto);

    /* Asociar el socket a la dirección y al puerto */
    if (bind(servidor, (struct sockaddr *)&dir, sizeof dir) < 0) {
        perror("Error en bind");
        close(servidor);
        return 1;
    }

    /* Poner el socket en estado de escucha */
    if (listen(servidor, 5) < 0) {
        perror("Error en listen");
        close(servidor);
        return 1;
    }

    printf("Servidor escuchando en el puerto %d...\n", puerto);

    /* Aceptar y atender clientes de a uno */
    while (1) {
        struct sockaddr_in cliente;
        socklen_t tam = sizeof cliente;

        int conexion = accept(servidor, (struct sockaddr *)&cliente, &tam);

        if (conexion < 0) {
            perror("Error en accept");
            continue;
        }

        /* Obtener y mostrar los datos del cliente */
        char ip[INET_ADDRSTRLEN];

        inet_ntop(AF_INET, &cliente.sin_addr, ip, sizeof ip);

        unsigned puerto_cliente = ntohs(cliente.sin_port);

        printf("Cliente conectado: %s:%u\n", ip, puerto_cliente);

        /* Recibir los datos y devolverlos al cliente */
        char buffer[1024];
        ssize_t recibidos;

        while ((recibidos = recv(conexion, buffer, sizeof buffer, 0)) > 0) {

            if (enviar_todo(conexion, buffer, (size_t)recibidos) < 0) {
                perror("Error al enviar datos");
                break;
            }
        }

        if (recibidos == 0) {
            printf("Cliente desconectado: %s:%u\n", ip, puerto_cliente);
        } else if (recibidos < 0) {
            perror("Error al recibir datos");
        }

        /* Cerrar la conexión con el cliente */
        close(conexion);
    }

    close(servidor);

    printf("\n");
    return 0;
}
