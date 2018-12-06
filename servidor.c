#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include <netdb.h>
#include <string.h>

#define BACKLOG 3

int main(int argc, char *argv[]){

    int port;
    sscanf(argv[1], "%d", &port);
    int id, new_id[2], i; // descriptores servidor, cliente
    int tam;
    char buffer[500];
    char mensaje[700];
    struct sockaddr_in server , client[2];
    char usuario1[20], usuario2[20];
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
    //puts("Socket creado");

    if( bind(id, (struct sockaddr *)&server , sizeof(server)) < 0){
        perror("Bind fallo. Error\n");
        return 1;
    }
    //puts("Bind listo\n");

    puts("Esperando por conexiones...\n");
    tam = sizeof(struct sockaddr_in);

    listen(id, 3); // comienzo a escuchar

    for(i = 0; i < 2; i++){

        new_id[i] = accept(id, (struct sockaddr *)&client[i], (socklen_t*)&tam); // si llega algo lo acepto
        printf("Se obtuvo una conexion desde %s\n", inet_ntoa(client[i].sin_addr)); // Asi se obtiene informacion de red del cliente, podrian aplicar otra cosa
    }

    for(i = 0; i < 2; i++){
         read(new_id[i], buffer, 500);
         write(new_id[i], "", 1);
         printf("%s se ha conectado\n", buffer);

        if(i == 0){
            strcpy(usuario1, buffer);
        }else {
            strcpy(usuario2, buffer);
        }
        memset(buffer, 0, 500);
    }
    int rec;
    while(1){

        rec = recv(new_id[0], buffer, sizeof(buffer), MSG_PEEK);

        // if(rec <= 0){
        //     printf("%s se ha desconectado\n", usuario1);
        // }
        
        sprintf(mensaje, "--> %s dice: %s", usuario1, buffer);
        send(new_id[1], mensaje, sizeof(mensaje), 0);
        //printf("enviado a %d\n", new_id[1]);
        memset(buffer, 0, 500);
        memset(mensaje, 0, 700);

        rec = recv(new_id[1], buffer, sizeof(buffer), MSG_PEEK);

        // if(rec <= 0){
        //     printf("%s se ha desconectado\n", usuario1);
        // }

        sprintf(mensaje, "--> %s dice: %s", usuario2, buffer);
        send(new_id[0], mensaje, sizeof(mensaje), 0);
        //printf("enviado a %d\n", new_id[0]);
        memset(buffer, 0, 500);
        memset(mensaje, 0, 700);

    }
    close(new_id[0]);
    close(new_id[1]);
}
