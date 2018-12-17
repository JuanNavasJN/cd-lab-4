start:
	gcc servidor.c -o servidor -pthread
	gcc cliente1.c -o cliente1 -pthread
	gcc cliente2.c -o cliente2 -pthread