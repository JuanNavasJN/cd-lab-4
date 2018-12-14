#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include<arpa/inet.h>	//inet_addr
#include<stdio.h>	//printf
#include <stdlib.h>
#include<unistd.h>    //write
#include <string.h>
#include <pthread.h>
//ESTRUCUTRA PARA SOCKET-THREAD
struct client_socket_thread{
        char buffer [256];
        int sockfd;
        pthread_t tid;
        pthread_attr_t attr;
        int rw;
};

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
                    printf("%s\n", car->buffer);
                }
        } else {
          //HILO QUE ENVIA
        while(1){
                bzero(car->buffer, 256);
                fgets(car->buffer, 255, stdin);
                n = write(car->sockfd, car->buffer, strlen((char *) &(car->buffer)));
                if (n < 0){
                        error("ERROR writing to socket");
                }

        }
        }
}

int main (int argc, char *argv[]){
    int id; //Descriptores servidor, cliente
    char buffer[500];
    char nombre[500];
    char usuariochat[500];
    struct client_socket_thread client_write_t, client_read_t;
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

    printf("Escriba su nombre de usuario: \n");
    fgets(nombre,500,stdin);
    send(client_write_t.sockfd, nombre, (strlen(nombre)) , 0);

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

    return 0;

}
