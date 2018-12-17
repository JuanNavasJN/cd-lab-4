#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>	//inet_addr
#include <stdio.h>	//printf
#include <stdlib.h>
#include <unistd.h>    //write
#include <string.h>
#include <pthread.h>
#include <dirent.h>

#define LENGTH 2000
#define LENGTH_NAME 200
#define LENGTH_BUFFER 500

//ESTRUCUTRA PARA SOCKET-THREAD
struct client_socket_thread{
        char buffer [256],nombre1 [256],nombre2 [256];
        int sockfd;
        pthread_t tid;
        pthread_attr_t attr;
        int rw;
};
struct client_socket_thread client_write_t, client_read_t;

void error(char *msg){
        perror(msg);
        exit(1);
}
void listarPropios()
{
    DIR *d;
    struct dirent *dir;
    d = opendir("./client1/");
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if(strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..") ) printf("%s\n", dir->d_name);
        }
        closedir(d);
    }
}

void listarServidor()
{
    DIR *d;
    struct dirent *dir;
    d = opendir("./server/");
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if(strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..") ) printf("%s\n", dir->d_name);
        }
        closedir(d);
    }
}

void* client_runner_fun(void* args){

        int n;
        struct client_socket_thread *car = (struct client_socket_thread*) args;
        if(car->rw == 0){
          //HILO QUE ESCUCHA
        while(1){
                bzero(car->buffer, 256);
                n = read(car->sockfd, car->buffer, 255);
                if (n < 0){
                        error("ERROR reading from socket");
                }
                if ((strcmp(car->buffer,"EXIT\n") == 0)||(strcmp(car->buffer,"EXIT ") == 0)){
                //   pthread_cancel(client_write_t.tid);
                  pthread_exit(NULL);
                  break;
                }
                printf("%s dice: %s\n",car->nombre2, car->buffer);

                }
        } else {
          //HILO QUE ENVIA
        while(1){
                bzero(car->buffer, 256);
                fgets(car->buffer, 255, stdin);
                n = write(car->sockfd, car->buffer, strlen((char *) &(car->buffer)));
                if ((strcmp(car->buffer,"EXIT\n") == 0)||(strcmp(car->buffer,"EXIT ") == 0)){
                //   pthread_cancel(client_read_t.tid);
                  pthread_exit(NULL);
                  break;
                }
                if (n < 0){
                        error("ERROR writing to socket");
                }
        }
        }
}

void recibirArchivo(int id, char *nombreArchivo){
    
    // Envia nombre del archivo a descargar
    char temp[LENGTH_NAME];
    bzero(temp, LENGTH_NAME);
    int len = strlen(nombreArchivo);
    strncpy(temp,nombreArchivo,len-1);
    send(id, temp, sizeof(temp), 0); // Enviar nombre del archivo
    float count = 0;
    char status[1];

    // recv(id, status, sizeof(status), 0);
    // if(strcmp(status, "1") == 0){
    //     printf("Descargando...\n");
    // }else{
    //     printf("\nArchivo no existe en el servidor\n");
    //     return;
    // }

    /*Recibe archivo desde el cliente */
    char revbuf[LENGTH]; // Receiver buffer
    char fr_name[LENGTH_NAME + 10];
    sprintf(fr_name, "./client1/%s", temp);
    
    FILE *fr = fopen(fr_name, "a");
    if(fr == NULL){
        printf("File %s Cannot be opened file on client.\n", fr_name);
        return;
    }
        
    else
    {
        bzero(revbuf, LENGTH); 
        int fr_block_sz = 0;
        //int success = 0;
        //while(success == 0)
        //{
            count = 0;
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
                // else if(fr_block_sz)
                // {
                // 	break;
                // }
                count += fr_block_sz;
                printf("%f KB descargados\n", count/1000);

                if(fr_block_sz < LENGTH ) break;
                bzero(revbuf, LENGTH);
            }
            printf("Ok archivo descargado!\n");
            fclose(fr);
        //}
    }
}

void enviarArchivo(int id, char *nombreArchivo){
    /* Enviar archivo al servidor */
    float count = 0;
    char fs_name[LENGTH_NAME + 10];
    char temp[LENGTH_NAME];
    bzero(temp, LENGTH_NAME);
    int len = strlen(nombreArchivo);
    strncpy(temp,nombreArchivo,len-1);
    send(id, temp, sizeof(temp), 0); // Enviar nombre del archivo
    
    sprintf(fs_name, "./client1/%s", temp);
    //char* fs_name = "./client/conest.png";
    char sdbuf[LENGTH + 10]; 
    printf("[Cliente] Enviando %s al servidor...\n", fs_name);

    FILE *fs = fopen(fs_name, "r");
    if(fs == NULL)
    {
        printf("ERROR: File %s not found.\n", fs_name);
        return;
    }
    bzero(sdbuf, LENGTH); 
    int fs_block_sz; 
    //int success = 0;
    count = 0;
    while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs)) > 0)
    {
        if(send(id, sdbuf, fs_block_sz, 0) < 0)
        {
            printf("ERROR: Failed to send file %s.\n", fs_name);
            break;
        }
        count += fs_block_sz;
        printf("%f KB subidos\n", count/1000);
        bzero(sdbuf, LENGTH);
    }
    fclose(fs);

    bzero(sdbuf, LENGTH);
    sprintf(sdbuf, "%s", "enviado");

    if(send(id, sdbuf, strlen(sdbuf), 0) <= 0){
        perror("Error enviado\n");
    };

    printf("\nOk Archivo %s ya se envio desde el cliente!\n", fs_name);
}
void runChat(){

    char nombre[500];
    printf("Escriba su nombre de usuario: \n");
    int t1, t2;
     
    fgets(nombre,500,stdin);
// printf("%d\n", strlen(nombre) - 1);
    if(send(client_write_t.sockfd, nombre, (strlen(nombre) - 1) , 0) < 0){
            printf("Error\n");
    }
    printf("Comienza a chatear: \n");
    memset(nombre, 0, strlen(nombre));
    recv(client_write_t.sockfd, nombre, 255, 0);
    strncpy(client_read_t.nombre2,nombre,strlen(nombre));
    //printf("TAMANO: %d", strlen(client_read_t.nombre2));

    sleep(1);//SINCRONIZACION CON SERVIDOR

    pthread_attr_init(&client_write_t.attr);
    pthread_attr_init(&client_read_t.attr);
    client_write_t.rw = 1;
    client_read_t.rw = 0;
    //HILO QUE ENVIA
    pthread_create(&client_write_t.tid, &client_write_t.attr, client_runner_fun, &client_write_t);
    //HILO QUE RECIBE
    pthread_create(&client_read_t.tid, &client_read_t.attr, client_runner_fun, &client_read_t);

    pthread_join(client_write_t.tid, NULL);
    pthread_join(client_read_t.tid, NULL);
}


int mostrarOpciones(int id){
    int opt;
    char nombreArchivo[100];
    char optc[10];
    printf("%d\n", id);
    printf("Menu: \n");
    printf(" 1 - Chat room\n");
    printf(" 2 - Subir archivos\n");
    printf(" 3 - Descargar archivos\n");
    printf(" 4 - Listar mis archivos\n");
    printf(" 5 - Listar archivos del servidor\n");
    printf("Seleccione una opcion: ");

    do{
        fgets(optc, 10, stdin);
        opt = atoi(optc);
    }while(opt != 1 && opt != 2 && opt != 3 && opt != 4 && opt != 5);
    
    bzero(optc, 10);
    bzero(nombreArchivo, sizeof(nombreArchivo));

    switch(opt){

        case 1: 
            printf("----------- Chat room --------\n");
            sprintf(optc, "chat");
            send(id, optc, sizeof(optc), 0); // Enviar opcion
            runChat();
            break;
        case 2:
            sprintf(optc, "subir");
            send(id, optc, sizeof(optc), 0); // Enviar opcion
            printf("----------- Subir archivo --------\n");
            printf("Ingrese el nombre del archivo a subir: ");
            fgets(nombreArchivo, sizeof(nombreArchivo), stdin);
            enviarArchivo(id, nombreArchivo);
            printf("----------------------------------\n");
            break;
        case 3:
            sprintf(optc, "bajar");
            send(id, optc, sizeof(optc), 0); // Enviar opcion
            printf("----------- Descargar archivo --------\n");
            printf("Ingrese el nombre del archivo a descargar: ");
            fgets(nombreArchivo, sizeof(nombreArchivo), stdin);
            recibirArchivo(id, nombreArchivo);
            printf("----------------------------------\n");
            break;
        case 4:
            printf("---------- Listar mis archivos --------------\n");
            listarPropios();
            printf("---------------------------------------\n");
            break;
        case 5:
            printf("---------- Listar archivos del servidor --------------\n");
            listarServidor();
            printf("----------------------------------\n");
            break;
        default:
            printf("Opcion invalida\n");
            break;
    }
    return 0;
}


int main (int argc, char *argv[]){
    int id; //Descriptores servidor, cliente
    char buffer[500];
    char usuariochat[500];
    
    struct sockaddr_in server; //int servidor
    struct hosten *he; // lo necesitare para el nombre de host o la ip

    int port;
    sscanf(argv[2], "%d", &port);

    //estructura server del SERVIDOR

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(port);// el puerto que usare (el segundo argumento de la llamada)
    bzero(&(server.sin_zero),8);

    //socket del cliente
    client_write_t.sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bcopy(&client_write_t.sockfd, &client_read_t.sockfd, sizeof(client_write_t.sockfd));//MEMSET
    if( client_write_t.sockfd == -1){
        printf("No se pudo crear socket\n");
    }

    if (connect(client_write_t.sockfd , (struct sockaddr *)&server , sizeof(server)) < 0){
		    perror("Fallo la conexion. Error\n");
		    return 1;
	  }
    
    while(1){

        mostrarOpciones(client_write_t.sockfd);
           
    }


    return 0;
}
