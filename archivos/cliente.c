#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include<arpa/inet.h>	//inet_addr
#include<stdio.h>	//printf
#include <stdlib.h>
#include<unistd.h>    //write
#include <string.h>
#include <dirent.h>
#include <errno.h>

#define LENGTH 2000
#define LENGTH_NAME 200
#define LENGTH_BUFFER 500

void listarPropios()
{
    DIR *d;
    struct dirent *dir;
    d = opendir("./client/");
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

int solicitarUsername(int id){
    char nombre[50];
    printf("Escriba su nombre de usuario:  \n");
    fgets(nombre, sizeof(nombre), stdin);

    if(send(id, nombre, strlen(nombre) - 1, 0) > 0) return 1;
    else return 0;
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
    sprintf(fr_name, "./client/%s", temp);
    
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
    
    sprintf(fs_name, "./client/%s", temp);
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

void mostrarOpciones(int id){
    int opt;
    char nombreArchivo[100];
    char optc[10];

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
}

void conectar(char *ip, int port){

    int id; //Descriptores servidor, cliente
    char buffer[LENGTH_BUFFER];
    char nombre[LENGTH_BUFFER];
    char usuariochat[LENGTH_BUFFER];

    struct sockaddr_in server; //int servidor

    struct hosten *he; // lo necesitare para el nombre de host o la ip

    //estructura server del SERVIDOR

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(port);// el puerto que usare (el segundo argumento de la llamada)
    bzero(&(server.sin_zero),8);

    //socket del cliente
    id = socket(AF_INET, SOCK_STREAM, 0);
    if( id == -1){
        printf("No se pudo crear socket\n");
    }
    //puts("Socket creado\n");

    if (connect(id , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("Fallo la conexion. Error\n");
		exit(1);
	}
    //puts("Conectado\n");
    // ya en teoria la conexion al server esta hecha

    while(1){
        mostrarOpciones(id);
    }


    // if(!solicitarUsername(id)){
    //     perror("Error solicitando username\n");
    //     exit(1);
    // }

    //printf("Comienza a chatear\n");

    while(1){

        bzero(buffer, LENGTH_BUFFER);

        printf("<-- \n");

        //scanf("%s", buffer);
        fgets(buffer, sizeof(buffer), stdin);

        if( send(id, buffer, strlen(buffer) , 0) < 0){
            puts("Envio fallido");
            exit(1);
        }else{
             //printf("<-- ");
        }
        bzero(buffer, LENGTH_BUFFER); 

        do{
            recv(id, buffer, 500, 0);
        }while(!strcmp(buffer, ""));
        
        printf("%s\n", buffer);

        if(strcmp(buffer, "EXIT") == 0){
            close(id);
            exit(1);
        }
        
    }
    close(id); //Importante cerrar la conexion

}

int main (int argc, char *argv[]){

    int port;
    sscanf(argv[2], "%d", &port);
    
    conectar(argv[1], port);
    
}
