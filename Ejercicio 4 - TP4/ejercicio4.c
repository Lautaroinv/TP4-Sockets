/*
Ejercicio 4
Objetivo: implementar la secuencia básica de un servidor TCP: socket(), bind(), listen() y accept().

Consigna: escribir el programa ej04_saludo_servidor.c, que se ejecuta como
./ej04_saludo_servidor <puerto> y:
1.​ Cree un socket TCP y lo asocie a la dirección comodín y al puerto indicado.
2.​ Por cada cliente que se conecte, muestre en pantalla su dirección IP y su puerto.
3.​ Le envíe al cliente el mensaje Hola, te conectaste desde <IP>:<puerto>.
4.​ Cierre la conexión con ese cliente y quede esperando al siguiente, sin terminar.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {

    printf("\n");

    /* Verificar que se haya indicado el puerto */
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <puerto>\n", argv[0]);
        return 1;
    }

    int puerto = atoi(argv[1]);

    /* Crear el socket TCP */
    int servidor = socket(AF_INET, SOCK_STREAM, 0);

    if (servidor < 0) {
        perror("Error al crear el socket");
        return 1;
    }

    /* Configurar la direccion del servidor */
    struct sockaddr_in direccion = {0};

    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(puerto);

    /* Asociar el socket a la direccion y al puerto */
    if (bind(servidor, (struct sockaddr *)&direccion,
             sizeof(direccion)) < 0) {
        perror("Error en bind");
        close(servidor);
        return 1;
    }

    /* Preparar el socket para escuchar conexiones */
    if (listen(servidor, 5) < 0) {
        perror("Error en listen");
        close(servidor);
        return 1;
    }

    printf("Servidor escuchando en el puerto %d...\n", puerto);

    /* Aceptar clientes, atenderlos y continuar esperando */
    while (1) {

        struct sockaddr_in cliente;
        socklen_t longitud = sizeof(cliente);

        int nuevo = accept(servidor,
                           (struct sockaddr *)&cliente,
                           &longitud);

        if (nuevo < 0) {
            perror("Error en accept");
            continue;
        }

        char ip[INET_ADDRSTRLEN];

        inet_ntop(AF_INET, &cliente.sin_addr, ip, sizeof(ip));

        int puerto_cliente = ntohs(cliente.sin_port);

        printf("\nCliente conectado: %s:%d\n", ip, puerto_cliente);

        /* Construir y enviar el mensaje de bienvenida */
        char mensaje[128];

        snprintf(mensaje, sizeof(mensaje),
                 "\nHola, te conectaste desde %s:%d\n\n",
                 ip, puerto);

        if (send(nuevo, mensaje, strlen(mensaje), 0) < 0) {
            perror("Error al enviar el mensaje");
        }

        /* Cerrar la conexion con el cliente */
        close(nuevo);

        printf("Cliente desconectado: %s:%d\n\n",
               ip, puerto_cliente);
    }

    close(servidor);

    printf("\n");

    return 0;
}
