
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
    /* 
    Creamos un arreglo para almacenar los descriptores de los sockets que se crearan. Luego, se crean
    los sockets indicados por el ejercicio y se muestran su descriptores.
    
    Los dejamos abiertos a la vez para ver cómo se numeran */
    printf("\n");  

    int fds[3];
    fds[0] = crear("TCP sobre IPv4", AF_INET,  SOCK_STREAM, 0);
    fds[1] = crear("UDP sobre IPv4", AF_INET,  SOCK_DGRAM,  0);
    fds[2] = crear("TCP sobre IPv6", AF_INET6, SOCK_STREAM, 0);


    /* 
    Consigna:​ ¿Qué sucede con la numeración si se cierra el primer socket y se crea uno nuevo?
    Comprobarlo modificando el programa.
    */

    /* Cerramos solamente el primer socket tal como indica la segunda parte del ejercicio*/

    if (fds[0] >= 0) {
        if (close(fds[0]) < 0) {
            perror("Error al cerrar el primer socket");
        } else {
            printf("Se cerro el primer socket (descriptor %d)\n", fds[0]);
        }
    }

    /* Creamos un nuevo socket luego de cerrar el anterior para comprobar que sucede con
    la numeracion*/
    int nuevo = crear("\nNuevo TCP sobre IPv4", AF_INET, SOCK_STREAM, 0);
    printf("\n");

    /* Cerramos todos los sockets */

    for (int i = 1; i < 3; i++) {
        if (fds[i] >= 0 && close(fds[i]) < 0) {
            perror("Error al cerrar el socket");
        }
    }

    if (nuevo >= 0 && close(nuevo) < 0) {
        perror("Error al cerrar el nuevo socket");
    }


    /* 
    Consigna: ¿Qué error se obtiene con socket(AF_INET, SOCK_DGRAM, IPPROTO_TCP)?
    ¿Por qué?
    
    Combinación inválida: */
    
    int malo = crear("AF_INET, SOCK_DGRAM, IPPROTO_TCP", AF_INET, SOCK_DGRAM, IPPROTO_TCP);
    if (malo >= 0){
        close(malo);
    }
    printf("\n");    

    return 0;
}

