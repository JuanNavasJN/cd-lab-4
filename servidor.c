#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include <netdb.h>

//#define PORT 3006  // De verdad no hacer esto, tratar de no definir nada
#define BACKLOG 2

int main(int argc, char *argv[]){

    int port;
    sscanf(argv[1], "%d", &port);
    int id, new_id; // descriptores servidor, cliente
    int tam;
    char buffer[100];
    struct sockaddr_in server , client;
    //estructura server del SERVIDOR

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port); // el puerto que usare
    bzero(&(server.sin_zero), 8);

    //socket del servidor
    id = socket(AF_INET, SOCK_STREAM, 0);
    if( id == -1){
        printf("No se ha podido crear el socket\n");
    }
    puts("Socket creado");

    if( bind(id, (struct sockaddr *)&server , sizeof(server)) < 0){
        perror("Bind fallo. Error\n");
        return 1;
    }
    puts("Bind listo\n");
    
    listen(id, BACKLOG); // comienzo a escuchar

    puts("Esperando por conecciones...\n");

    while(1){
        tam = sizeof(struct sockaddr_in);
        new_id = accept(id, (struct sockaddr *)&client, (socklen_t*)&tam); // si llega algo lo acepto
        printf("Se obtuvo una conexion desde %d\n", inet_ntoa(client.sin_addr)); // Asi se obtiene informacion de red del cliente, podrian aplicar otra cosa
        read(new_id, buffer, 100);
        printf("\n");
        printf("Mensaje del servidor: %s\n", buffer);
    }
    close(new_id);
}