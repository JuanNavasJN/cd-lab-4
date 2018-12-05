#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include <netdb.h>
#include <string.h>

//#define PORT 3006  // De verdad no hacer esto, tratar de no definir nada
#define BACKLOG 3

int main(int argc, char *argv[]){

    int port;
    sscanf(argv[1], "%d", &port);
    int id, new_id[2]; // descriptores servidor, cliente
    int tam;
    char buffer[500];
    char buffer2[500];
    struct sockaddr_in server , client[2];
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

    int i;

    for(i = 0; i < 2; i++){

        new_id[i] = accept(id, (struct sockaddr *)&client[i], (socklen_t*)&tam); // si llega algo lo acepto
        printf("Se obtuvo una conexion desde %s\n", inet_ntoa(client[i].sin_addr)); // Asi se obtiene informacion de red del cliente, podrian aplicar otra cosa

    //  listen(id2, BACKLOG); // comienzo a escuchar
    //  new_id2 = accept(id2, (struct sockaddr *)&client2, (socklen_t*)&tam); // si llega algo lo acepto
    //  printf("HOLA3\n");
    //  printf("Se obtuvo una conexion desde %s\n", inet_ntoa(client2.sin_addr)); // Asi se obtiene informacion de red del cliente, podrian aplicar otra cosa

         read(new_id[i], buffer, 500);
         write(new_id[i], "", 1);
         printf ("%s se ha conectado\n", buffer);

         if(i > 0){
            send(new_id[0],buffer,strlen(buffer),0);
         }
    //printf("Se conecta %s\n", buffer);
    //read(new_id2, buffer2, 500);
    //printf("Se conecta %s\n", buffer2);
    //close(new_id2);
           // memset(buffer, 0, 500);
            //recv(new_id[i], buffer, 1024, 0);
            

    //send(new_id2,buffer,1024,0);//Se manda el nombre de usuario 1 a cliente2
    //send(new_id,buffer2,1024,0);//Se manda el nombre del usuario 2 a cliente1
            //send(new_id2,buffer,1024,0);
            memset(buffer, 0, 500);
    }

    while(1){

      //  read(new_id, buffer, 500);
        //read(new_id, buffer, 500);
        // read(new_id, buffer, 500); //LEE DE CLIENTE1
        // if(strcmp(buffer, "EXIT") == 0){
        //     printf("%s\n", buffer);
        //     close(new_id2);
        //     return 1;
        // }
        // write(new_id2,buffer,1024);//ENVIA A CLIENTE2
        //printf("\n");
        //printf("Mensaje del cliente: %s\n", buffer);
        memset(buffer, 0, 500);
    }
    close(new_id[0]);
    close(new_id[1]);
  //  close(new_id2);
}
