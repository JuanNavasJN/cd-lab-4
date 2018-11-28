#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include<arpa/inet.h>	//inet_addr
#include<stdio.h>	//printf
#include <stdlib.h>

//#defien PORT 3005
//#define BACKLOG 2

int main (int argc, char *argv[]){
    int id; //Descriptores servidor, cliente
    char buffer[100] = "hola";

    struct sockaddr_in server; //int servidor

    struct hosten *he; // lo necesitare para el nombre de host o la ip

    //Necesario para obtener la ip. Existe otra funcion para esto que quiza necesiten...
    if((he=gethostbyname(argv[1])) == NULL){
        printf("error de gethostbyname()\n");
        exit(-1);
    }

    int port;
    sscanf(argv[2], "%d", &port);

    //estructura server del SERVIDOR

    server.sin_family = AF_INET;

    server.sin_addr = *((struct in_addr *)he->h_addr);
    //server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(port);// el puerto que usare (el segundo argumento de la llamada)
    bzero(&(server.sin_zero),8);

    //socket del cliente
    id = socket(AF_INET, SOCK_STREAM, 8);
    if( id == -1){
        printf("No se pudo crear socket\n");
    }
    puts("Socket creado\n");

    if (connect(id , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("Fallo la conexion. Error\n");
		return 1;
	}
    puts("Conectado\n");
    // ya en teoria la conexion al server esta hecha
    write(id, buffer, 100);
    close(id); //Importante cerrar la conexion

}