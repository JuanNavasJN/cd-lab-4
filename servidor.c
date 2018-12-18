#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>	//inet_addr
#include <stdio.h>	//printf
#include <stdlib.h>
#include <unistd.h>    //write
#include <string.h>
#include <pthread.h>

#define BACKLOG 3
#define LENGTH 10000
#define LENGTH_BUFFER 500
#define LENGTH_MENSAJE 700
#define LENGTH_NAME 200

//ESTRUCUTRA PARA SOCKET-THREAD
struct server_socket_thread{
    char buffer [256],nombre1 [256],nombre2 [256];
    int newsockfd,newsockfd2;
    pthread_t tid;
    pthread_attr_t attr;
        int rw,rw2;
};

struct server_socket_thread server_write_t, server_read_t;

void error(char *msg){
        perror(msg);
        exit(1);
}


//FUNCION QUE SIRVE DE RECEPTOR Y ENVIO
void* server_recibe_envia(void* args){
    int n;
    struct server_socket_thread *sar = (struct server_socket_thread*) args;
    if(sar->rw == 0){
      //HILO QUE ESCUCHA A CLIENTE2 Y ENVIA A CLIENTE1
      while(1){
        bzero(sar->buffer, 256);
        n = read(sar->newsockfd2, sar->buffer, 255);
        if (n < 0){
  //          error("ERROR reading from socket");
        }
        write(sar->newsockfd, sar->buffer, strlen((char *) &(sar->buffer)));
        //printf("%s\n", sar->buffer);
        if ((strcmp(sar->buffer,"EXIT\n") == 0)||(strcmp(sar->buffer,"EXIT ") == 0)){
          printf("Salio %s\n", sar->nombre2);

          pthread_exit(NULL);
          close(sar->newsockfd);
          break;
        }
        }
    } else {
      //HILO QUE ESCUCHA AL CLIENTE1 Y ENVIA A CLIENTE2
        while(1){
          bzero(sar->buffer, 256);
          n = read(sar->newsockfd, sar->buffer, 255);
          if (n < 0){
  //          error("ERROR reading from socket");
          }
          write(sar->newsockfd2, sar->buffer, strlen((char *) &(sar->buffer)));
          //printf("%s\n", sar->buffer);
          if ((strcmp(sar->buffer,"EXIT\n") == 0)||(strcmp(sar->buffer,"EXIT ") == 0)){
            printf("Salio %s\n", sar->nombre1);

            pthread_exit(NULL);
            close(sar->newsockfd2);
            break;
          }
        }
    }
}

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
                //usleep(10);
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
                //printf("%d\n", fr_block_sz);
                if(fr_block_sz < LENGTH &&  fr_block_sz > 0) break;
                bzero(revbuf, LENGTH);
                usleep(10);
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
    printf("[Servidor] Enviando %s al cliente...\n", fs_name);
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
        usleep(10);
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

int chatRoom(){
  // printf("En el chat room: %d, %d\n", server_write_t.newsockfd, server_write_t.newsockfd2);
  char buffer[500];
  char buffer2[500];

  bzero(buffer, 500);
  bzero(buffer2, 500);
  //RECIBE NOMBRES
  recv(server_write_t.newsockfd, buffer, (sizeof(buffer)), 0);
  strncpy(server_write_t.nombre1,buffer,strlen(buffer));
  printf("%s se ha conectado\n", server_write_t.nombre1);
  send(server_write_t.newsockfd2, buffer, (strlen(buffer)) , 0);

  recv(server_write_t.newsockfd2, buffer2, (sizeof(buffer2)), 0);
  strncpy(server_read_t.nombre2,buffer2,strlen(buffer2));
  printf ("%s se ha conectado\n", server_read_t.nombre2);
  send(server_write_t.newsockfd, buffer2, (strlen(buffer2)) , 0);

//  memset(buffer, 0, 500);
//  memset(buffer2, 0, 500);

  //INICIO HILOS
  pthread_attr_init(&server_write_t.attr);
  pthread_attr_init(&server_read_t.attr);
  server_write_t.rw = 1;
  server_read_t.rw = 0;
  bcopy(&server_write_t.newsockfd, &server_read_t.newsockfd, sizeof(server_write_t.newsockfd));
  bcopy(&server_write_t.newsockfd2, &server_read_t.newsockfd2, sizeof(server_write_t.newsockfd2));
  //ESCUCHA AL CLIENTE 1 Y ENVIA AL CLIENTE 2:
  pthread_create(&server_write_t.tid, &server_write_t.attr, server_recibe_envia, &server_write_t);
  //ESCUCHA AL CLIENTE 2 Y ENVIA AL CLIENTE 1:
  pthread_create(&server_read_t.tid, &server_read_t.attr, server_recibe_envia, &server_read_t);

  pthread_join(server_write_t.tid, NULL);
  //printf("SALE1\n");
  pthread_join(server_read_t.tid, NULL);
  //printf("SALE2\n");

  // printf("Saliendo del chat room...\n");

  return 1;
}

void *options(void *arg){
  int id = (int*) arg;
  //printf("Esperando opciones... %d\n", id);
  char buffer[LENGTH_BUFFER];
  int status;

  while(1){

       // printf("Esperando opcion...\n");
        bzero(buffer, sizeof(buffer));

        status = recv(id, buffer, sizeof(buffer), 0);
       // printf("%d\n", status);
        if( status <= 0){
            perror("Error recibiendo opcion\n");
            break;
        };

        //printf("%s\n", buffer);
        //pthread_exit(NULL);

        if(strcmp(buffer, "subir") == 0){
            printf("Esperando archivo...\n");
            recibirArchivo(id);
        }else if(strcmp(buffer, "bajar") == 0){
            printf("Preparando archivo...\n");
            enviarArchivo(id);
        }else if(strcmp(buffer, "chat") == 0){
           printf("Chat room...\n");
           //chatRoom();
           pthread_exit(NULL);
        }
        // close(new_id[0]);
        // exit(0);
    }

  pthread_exit(NULL);
}

int esperarOpciones(){

  pthread_t cliente1, cliente2;
  int cli1, cli2;

  cli1 = pthread_create(&cliente1, NULL, options,(void*) server_write_t.newsockfd);
  if(cli1){
    printf("Error hilo cliente1\n");
    exit(1);
  }
  cli2 = pthread_create(&cliente2, NULL, options,(void*) server_write_t.newsockfd2);
  if(cli2){
    printf("Error hilo cliente2\n");
    exit(1);
  }

  pthread_join(cliente1, NULL);
  pthread_join(cliente2, NULL);

  return 1;
}


int main(int argc, char *argv[]){

    int port;
    sscanf(argv[1], "%d", &port);
    int id, new_id, new_id2; // descriptores servidor, cliente
    int tam;
  
    struct sockaddr_in server , client;
    
    //estructura server del SERVIDOR

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port); // el puerto que usare
    bzero(&(server.sin_zero), 8);

    id = socket(AF_INET, SOCK_STREAM, 0);
    if( id == -1){
        printf("No se ha podido crear el socket\n");
    }

    if( bind(id, (struct sockaddr *)&server , sizeof(server)) < 0){
        perror("Bind fallo. Error\n");
        return 1;
    }

    puts("Esperando por conexiones...\n");
    tam = sizeof(struct sockaddr_in);

    listen(id, 3); // comienzo a escuchar

    server_write_t.newsockfd = accept(id, (struct sockaddr *)&client, (socklen_t*)&tam); // si llega algo lo acepto
    printf("Se obtuvo una conexion desde %s\n", inet_ntoa(client.sin_addr)); // Asi se obtiene informacion de red del cliente, podrian aplicar otra cosa

    server_write_t.newsockfd2= accept(id, (struct sockaddr *)&client, (socklen_t*)&tam); // si llega algo lo acepto
    printf("Se obtuvo una conexion desde %s\n", inet_ntoa(client.sin_addr));

   //---------------------------------------------------------------------------------------------------------------------

    while(1){
      if(esperarOpciones() == 1){
        chatRoom();
      }
    }
    
    
    return 0;
}
