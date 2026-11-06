/* Ejercicio 3: 
Consigna: escribir el programa ej03_resolver.c, que se ejecuta como ./ej03_resolver <host>
<servicio> y:
1.​ Llame a getaddrinfo() solicitando direcciones de cualquier familia (AF_UNSPEC)
para sockets de tipo SOCK_STREAM.
2.​ Recorra la lista devuelta y, para cada nodo, muestre la familia (IPv4 o IPv6), la
dirección IP en texto y el número de puerto.
3.​ Para cada dirección, obtenga su nombre inverso con getnameinfo(). Si no existe,
debe indicarlo.
4.​ Libere la lista con freeaddrinfo().
5.​ Informe los errores de resolución con gai_strerror().
Pruebas mínimas: localhost http, google.com https, example.com 8080 y un nombre
inexistente.

*/
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#ifndef NI_MAXHOST
#define NI_MAXHOST 1025
#endif

int main(int argc, char *argv[]) {
    
    printf("\n");

    if (argc != 3) {
        fprintf(stderr, "Uso: %s <host> <servicio>\n", argv[0]);
        return 1;
    }

    /* Configura los parámetros para solicitar direcciones IPv4 o IPv6 correspondientes a sockets TCP. */

    struct addrinfo hints, *lista;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;        /* IPv4 o IPv6 */
    hints.ai_socktype = SOCK_STREAM;

    /* Resuelve el nombre del host y el servicio, e informa los errores mediante gai_strerror(). */

    int err = getaddrinfo(argv[1], argv[2], &hints, &lista);
    if (err != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        return 1;
    }

    /* Recorre la lista de direcciones obtenidas para procesar cada resultado. */

    int n = 0;
    for (struct addrinfo *p = lista; p != NULL; p = p->ai_next) {
        char ip[INET6_ADDRSTRLEN];
        void *addr;
        unsigned puerto;

        /* Identifica si la dirección es IPv4 o IPv6, obtiene su IP y puerto, y convierte la dirección a texto. */

        if (p->ai_family == AF_INET) {
            struct sockaddr_in *s4 = (struct sockaddr_in *)p->ai_addr;
            addr = &s4->sin_addr;
            puerto = ntohs(s4->sin_port);
        } else {
            struct sockaddr_in6 *s6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &s6->sin6_addr;
            puerto = ntohs(s6->sin6_port);
        }
        inet_ntop(p->ai_family, addr, ip, sizeof ip);

        /* Extra: resolución inversa 
        Obtiene el nombre de host asociado a cada dirección IP e indica si no existe un nombre inverso.
        */
        char nombre[NI_MAXHOST];
        if (getnameinfo(p->ai_addr, p->ai_addrlen, nombre, sizeof nombre,
                        NULL, 0, NI_NAMEREQD) != 0)
            strcpy(nombre, "(sin nombre inverso)");

        /* Muestra la familia IP, la dirección, el puerto y el nombre inverso de cada resultado. */
        
        printf("[%d] %s  %-40s puerto %-5u  inverso: %s\n", ++n,
               p->ai_family == AF_INET ? "IPv4" : "IPv6", ip, puerto, nombre);
    }

    /* Libera la memoria utilizada por la lista de direcciones obtenidas. */

    freeaddrinfo(lista);

    printf("\n");

    return 0;
}
