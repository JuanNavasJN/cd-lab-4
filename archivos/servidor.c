#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include <netdb.h>
#include <string.h>
#include <errno.h>

#define BACKLOG 3
#define LENGTH 1000
#define LENGTH_BUFFER 500
#define LENGTH_MENSAJE 700
#define LENGTH_FILE 200


void recibirArchivo(int id){

    // Recibe nombre del archivo
    char fs_name[LENGTH_FILE];
    bzero(fs_name, LENGTH_FILE);
    if(recv(id, fs_name, sizeof(fs_name), 0) < 0){
            perror("Error recibiendo nombre de archivo\n");
    };
    
    /*Recibe archivo desde el cliente */
    char revbuf[LENGTH]; // Receiver buffer
    char fr_name[LENGTH_FILE];
    sprintf(fr_name, "./server/%s", fs_name);
    
    FILE *fr = fopen(fr_name, "a");
    if(fr == NULL)
        printf("File %s Cannot be opened file on server.\n", fr_name);
    else
    {
        bzero(revbuf, LENGTH); 
        int fr_block_sz = 0;
        //int success = 0;
        //while(success == 0)
        //{
            while(fr_block_sz = recv(id, revbuf, LENGTH, 0)) //could it be sockfd?
            {
                if(strcmp(revbuf, "enviado") == 0){
                    break;
                }
                if(fr_block_sz < 0)
                {
                    perror("Error receiving file from client to server.\n");
                }
                int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fr);
                if(write_sz < fr_block_sz)
                {
                    perror("File write failed on server.\n");
                }
                else if(fr_block_sz)
                {
                	break;
                }
                bzero(revbuf, LENGTH);

            }
            printf("Ok archivo recibido!\n");
            fclose(fr);
        //}
    }
}

int main(int argc, char *argv[]){

    int port;
    sscanf(argv[1], "%d", &port);
    int id, new_id[2], i; // descriptores servidor, cliente
    int tam;
    char buffer[LENGTH_BUFFER];
    char mensaje[LENGTH_MENSAJE];
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

    for(i = 0; i < 1; i++){

        new_id[i] = accept(id, (struct sockaddr *)&client[i], (socklen_t*)&tam); // si llega algo lo acepto
        printf("Se obtuvo una conexion desde %s\n", inet_ntoa(client[i].sin_addr)); // Asi se obtiene informacion de red del cliente, podrian aplicar otra cosa
    }
    bzero(buffer, LENGTH_BUFFER);

    while(1){

        if(recv(new_id[0], buffer, sizeof(buffer), 0) < 0){
            perror("Error recibiendo opcion\n");
        };

        if(strcmp(buffer, "subir") == 0){
            printf("Esperando archivo...\n");
            recibirArchivo(new_id[0]);
        }
    }

    // printf("A recibir...\n");
    // recibirArchivo(new_id[0]);

    // for(i = 0; i < 1; i++){
        
    //      read(new_id[i], buffer, 500);
    //      //write(new_id[i], "", 1);
    //      printf("%s se ha conectado\n", buffer);

    //     if(i == 0){
    //         strcpy(usuario1, buffer);
    //     }else {
    //         strcpy(usuario2, buffer);
    //     }
    //     memset(buffer, 0, 500);
    // }

    // int rec;
    // while(1){

    //     rec = recv(new_id[0], buffer, sizeof(buffer), MSG_PEEK);

    //     // if(rec <= 0){
    //     //     printf("%s se ha desconectado\n", usuario1);
    //     // }
        
    //     sprintf(mensaje, "--> %s dice: %s", usuario1, buffer);
    //     send(new_id[1], mensaje, sizeof(mensaje), 0);
    //     //printf("enviado a %d\n", new_id[1]);
    //     memset(buffer, 0, 500);
    //     memset(mensaje, 0, 700);

      
    // }
    close(new_id[0]);
    //close(new_id[1]);
    return 0;
}
