/* Ejercicio 1: crear sockets y manejar errores 
Consigna: escribir el programa ej01_sockets.c que:

1.​ Cree, sin cerrarlos todavía, tres sockets: TCP sobre IPv4, UDP sobre IPv4 y TCP sobre
IPv6, usando 0 como protocolo.
2.​ Muestre el descriptor obtenido para cada uno.
3.​ Cierre los tres sockets.
4.​ Intente crear un socket con la combinación socket(AF_INET, SOCK_STREAM, IPPROTO_UDP) y muestre el error obtenido.

Si la creación de un socket falla, el programa debe informar y continuar con los demás.

*/
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* Crea un socket, informa el resultado y devuelve el descriptor (o -1) */
static int crear(const char *desc, int familia, int tipo, int protocolo) {
    
    /* Se crea el socket */
    int fd = socket(familia, tipo, protocolo);

    /* Analiza el descriptor del socket creado, si es menor que 0, informa el error,
    sino, muestra la descripcion brindada junto con el descriptor del socket*/
    if (fd < 0) {
        fprintf(stderr, "%-35s -> ", desc);
        perror("error");            /* imprime el texto correspondiente a errno */
    } else {
        printf("%-35s -> descriptor %d\n", desc, fd);
    }

    /* Retorna el descriptor del socket creado*/
    return fd;
}

int main(void) {
   
    printf("\n");
    /* 
    Consigna 1 y 2
    Creamos un arreglo para almacenar los descriptores de los sockets que se crearan. Luego, se crean
    los sockets indicados por el ejercicio y se muestran su descriptores.
    
    Los dejamos abiertos a la vez para ver cómo se numeran */

    int fds[3];
    fds[0] = crear("TCP sobre IPv4", AF_INET,  SOCK_STREAM, 0);
    fds[1] = crear("UDP sobre IPv4", AF_INET,  SOCK_DGRAM,  0);
    fds[2] = crear("TCP sobre IPv6", AF_INET6, SOCK_STREAM, 0);

    /* 
    Consigna 3
    Se recorre todo el arreglo que almacena los descriptores de los sockets 
    para cerrar cada uno.
    */
    for (int i = 0; i < 3; i++){

        if (fds[i] >= 0){
            if (close(fds[i]) < 0) {
                perror("Error al cerrar el socket");
            }
        }
    }

    /* Consigna 4
    
    Combinación inválida: semántica de flujo con el protocolo UDP */
    
    int malo = crear("AF_INET, SOCK_STREAM, IPPROTO_UDP", AF_INET, SOCK_STREAM, IPPROTO_UDP);
    if (malo >= 0)
        close(malo);
    
    printf("\n");
    return 0;
}
