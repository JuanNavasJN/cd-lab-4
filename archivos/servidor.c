#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define BACKLOG 3
#define LENGTH 2000
#define LENGTH_BUFFER 500
#define LENGTH_MENSAJE 700
#define LENGTH_NAME 200


int recibirArchivo(int id){
    // Recibe nombre del archivo
    char fs_name[LENGTH_NAME];
    bzero(fs_name, LENGTH_NAME);
    if(recv(id, fs_name, sizeof(fs_name), 0) < 0){
            perror("Error recibiendo nombre de archivo\n");
    };
    /*Recibe archivo desde el cliente */
    char revbuf[LENGTH]; // Receiver buffer
    char fr_name[LENGTH_NAME + 10];
    sprintf(fr_name, "./server/%s", fs_name);
    
    FILE *fr = fopen(fr_name, "a");
    if(fr == NULL){
        printf("File %s Cannot be opened file on server.\n", fr_name);
        return 0; 
    }
        
    else
    {
        bzero(revbuf, LENGTH); 
        int fr_block_sz = 0;
        //int success = 0;
        //while(success == 0)
        //{
            while((fr_block_sz = recv(id, revbuf, LENGTH, 0)) > 0) //could it be sockfd?
            {
                // if(strcmp(revbuf, "enviado") == 0){
                //     break;
                // }
                if(fr_block_sz < 0)
                {
                    perror("Error receiving file from client to server.\n");
                    break;
                }
                int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fr);
                if(write_sz < fr_block_sz)
                {
                    perror("File write failed on server.\n");
                    break;
                }
                // else if(fr_block_sz == 0)
                // {
                // 	break;
                // }
                printf("%d\n", fr_block_sz);
                if(fr_block_sz < LENGTH &&  fr_block_sz > 0) break;
                bzero(revbuf, LENGTH);
            }
            printf("Ok archivo subido!\n");
            fclose(fr);
            return 1;
        //}
    }
}
int enviarArchivo(int id){
    // Recibe nombre del archivo
    char fs_name[LENGTH_NAME + 10];
    char temp[LENGTH_NAME];
    bzero(fs_name, LENGTH_NAME + 10);
    bzero(temp, LENGTH_NAME);
    if(recv(id, temp, sizeof(temp), 0) < 0){
            perror("Error recibiendo nombre de archivo\n");
    };
   
    sprintf(fs_name, "./server/%s", temp);
    //char* fs_name = "./client/conest.png";
    char sdbuf[LENGTH]; 
    printf("[Cliente] Enviando %s al cliente...\n", fs_name);
    char status[2];
    FILE *fs = fopen(fs_name, "r");
    if(fs == NULL)
    {
        printf("ERROR: File %s not found.\n", fs_name);
        //sprintf(status, "0");
        //send(id, status, sizeof(status), 0);
        return 0;
    }
    // else{
    //     sprintf(status, "1");
    //     send(id, status, sizeof(status), 0);
    // }
    bzero(sdbuf, LENGTH); 
    int fs_block_sz; 
    //int success = 0;

    while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs)) > 0)
    {
        //printf(".");
        if(send(id, sdbuf, fs_block_sz, 0) < 0)
        {
            printf("ERROR: Failed to send file %s.\n", fs_name);
            break;
        }
        bzero(sdbuf, LENGTH);
    }
    fclose(fs);

    // bzero(sdbuf, LENGTH);
    // sprintf(sdbuf, "%s", "enviado");

    // if(send(id, sdbuf, strlen(sdbuf), 0) <= 0){
    //     perror("Error enviado\n");
    // };

    printf("\nOk Archivo %s ya se envio desde el servidor!\n", fs_name);
    return 1;
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
    int status;
    while(1){

       // printf("Esperando opcion...\n");
        bzero(buffer, sizeof(buffer));

        status = recv(new_id[0], buffer, sizeof(buffer), 0);
       // printf("%d\n", status);
        if( status <= 0){
            perror("Error recibiendo opcion\n");
            break;
        };

        if(strcmp(buffer, "subir") == 0){
            printf("Esperando archivo...\n");
            recibirArchivo(new_id[0]);
        }else if(strcmp(buffer, "bajar") == 0){
            printf("Preparando archivo...\n");
            enviarArchivo(new_id[0]);
        }
        // close(new_id[0]);
        // exit(0);
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
