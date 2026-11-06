/* Ejercicio 2: el orden de bytes 

Consigna: escribir el programa ej02_endianness.c que:
1.​ Determine si la máquina es little-endian o big-endian, examinando el primer byte en
memoria de un uint16_t con valor 0x0102.
2.​ Muestre byte por byte, en hexadecimal, el número 8080 antes y después de aplicarle
htons().
3.​ Muestre byte por byte el valor de 32 bits 0xC0A8010A (la dirección 192.168.1.10)
antes y después de aplicarle htonl().
4.​ Asigne el valor 8080 al campo sin_port de un struct sockaddr_in sin convertirlo y
muestre el resultado de aplicarle ntohs(). Repetir asignándolo correctamente con
htons().

*/

#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <netinet/in.h>


/* 
Muestra los bytes de una variable en hexadecimal, recorriendo su memoria byte por byte. 
*/
static void mostrar_bytes(const char *etiqueta, const void *dato, size_t n) {
    const unsigned char *p = dato;
    printf("%-28s", etiqueta);
   
    for (size_t i = 0; i < n; i++)
        printf(" %02x", p[i]);
    printf("\n");
}

int main(void) {
    
    printf("\n");
    /* 1. Detectar el orden del host 
    Detecta si la máquina utiliza el orden de bytes big-endian o little-endian
    */
    uint16_t prueba = 0x0102;
    unsigned char primero = *(unsigned char *)&prueba;
    printf("Esta máquina es %s\n\n", primero == 0x01 ? "big-endian" : "little-endian");

    /* 2. Puerto de 16 bits 
    Compara la representación en memoria del puerto 8080 antes y después de convertirlo 
    al orden de bytes de red mediante htons()
    */
    uint16_t puerto = 8080;                 /* 0x1F90 */
    uint16_t puerto_red = htons(puerto);
    mostrar_bytes("8080 en orden de host:", &puerto, sizeof puerto);
    mostrar_bytes("8080 tras htons():", &puerto_red, sizeof puerto_red);

    /* 3. Valor de 32 bits: 192.168.1.10 = 0xC0A8010A 
    Compara la representación en memoria de una dirección IP de 32 bits antes y después 
    de convertirla al orden de bytes de red mediante htonl()
    */
    uint32_t ip = 0xC0A8010A;
    uint32_t ip_red = htonl(ip);
    mostrar_bytes("0xC0A8010A en orden de host:", &ip, sizeof ip);
    mostrar_bytes("0xC0A8010A tras htonl():", &ip_red, sizeof ip_red);

    /* 4. Error típico: olvidar htons() 
    Compara la asignación incorrecta y correcta de un puerto, mostrando la importancia 
    de utilizar htons() para convertirlo al orden de bytes de red.
    */
    struct sockaddr_in dir = {0};
    dir.sin_port = 8080;                    /* ¡sin convertir! */
    printf("\nsin_port = 8080 sin htons(): ntohs() devuelve %u\n", ntohs(dir.sin_port));
    dir.sin_port = htons(8080);             /* forma correcta */
    printf("sin_port = htons(8080):      ntohs() devuelve %u\n", ntohs(dir.sin_port));
   
    printf("\n");
    
    return 0;
}
