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

#define LENGTH 1000
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

void subirArchivo(int id, char *nombreArchivo){

    /* Enviar archivo al servidor */
        
    int count = 0;
    char fs_name[LENGTH_NAME];
    char temp[LENGTH_NAME];
    bzero(temp, LENGTH_NAME);
    int len = strlen(nombreArchivo);
    strncpy(temp,nombreArchivo,len-1);
    send(id, temp, sizeof(temp), 0); // Enviar nombre del archivo
    
    sprintf(fs_name, "./client/%s", temp);
    //char* fs_name = "./client/conest.png";
    char sdbuf[LENGTH]; 
    printf("[Cliente] Enviando %s al servidor...\n", fs_name);

    FILE *fs = fopen(fs_name, "r");
    if(fs == NULL)
    {
        printf("ERROR: File %s not found.\n", fs_name);
        exit(1);
    }
    bzero(sdbuf, LENGTH); 
    int fs_block_sz; 
    //int success = 0;

    while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs))>0)
    {
        if(send(id, sdbuf, fs_block_sz, 0) < 0)
        {
            printf("ERROR: Failed to send file %s.\n", fs_name);
            break;
        }
        printf(".");
        bzero(sdbuf, LENGTH);
    }
    fclose(fs);

    printf("\nOk Archivo %s ya se envio desde el cliente!\n", fs_name);
   
}

void mostrarOpciones(int id){

    int opt;
    char nombreArchivo[100];
    char *optc;

    printf("Menu: \n");
    printf(" 1 - Listar mis archivos\n");
    printf(" 2 - Listar archivos del servidor\n");
    printf(" 3 - Subir archivos\n");
    printf(" 4 - Descargar archivos\n");
    printf("Seleccione una opcion: ");

    do{
        fgets(optc, 5, stdin);
        opt = atoi(optc);
    }while(opt != 1 && opt != 2 && opt != 3 && opt != 4);
    
    bzero(optc, 7);

    switch(opt){
        case 1:
            printf("---------- Mis archivos --------------\n");
            listarPropios();
            printf("---------------------------------------\n");
            break;
        case 2:
            printf("---------- Servidor --------------\n");
            listarServidor();
            printf("----------------------------------\n");
            break;
        case 3:
            optc = "subir";
            send(id, optc, sizeof(optc), 0); // Enviar opcion
            printf("----------- Subir archivo --------\n");
            printf("Ingrese el nombre del archivo: ");
            fgets(nombreArchivo, sizeof(nombreArchivo), stdin);
            subirArchivo(id, nombreArchivo);
            printf("----------------------------------\n");
            break;
        case 4:
            printf("Descargar\n");
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
