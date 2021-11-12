/* Programacao Concorrente e Distribuida
 * Entrega 2
 * Atividade 1
 * Secao critica por espera ocupada */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// variaveis globais
int request, respond, soma;

/* Rotina de execucao da thread servidor */
void* servidor(void* arg){
	// loop forever
	while(1){
		// await request != 0
		while(request == 0) {}
		respond = request;
		// await respond = 0
		while(respond != 0) {}
		request = 0;
	}
}

/* Rotina de execucao da thread cliente */
void* cliente(void* arg){
	int id, local;

	// obtem o valor do id da thread cliente
	id = *((int*) arg);

	// loop forever
	while(1){
		// non-critical section
		while(respond != id) // busy wait
			request = id;
		
		// critical section
		local = soma;
		sleep(rand() % 2);
		soma = local + 1;
		printf("[%d]\t soma=%d\n", id, soma);

		respond = 0;
	}
}

/* Funcao principal */
int main(int argc, char** argv){
	int nThread, i;
	int* id;
	pthread_t* cliente_t;
	pthread_t servidor_t;

	// verifica o parametro da linha de comando
	// pega o numero de threads a ser usado
	if(argc != 2){
		printf("Uso correto: ./atv1 <numero de threads>.\n");
		return 0;
	}
	nThread = atoi(argv[1]);

	// inicializa as variaveis globais
	request = 0;
	respond = 0;
	soma = 0;

	// Aloca o vetor de threads clientes
	cliente_t = (pthread_t*) malloc(nThread * sizeof(pthread_t));

	// cria ids para as threads clientes
	id = (int*) malloc(nThread * sizeof(int));
	for(i = 1; i <= nThread; i++)
		id[i] = i;

	// inicia a thread servidor
	pthread_create(&servidor_t, NULL, servidor, NULL);

	// inicia as threads clientes
	for(i = 1; i <= nThread; i++)
		pthread_create(&cliente_t[i-1], NULL, cliente, (void*) &id[i]);

	pthread_join(servidor_t, NULL);
	for(i = 0; i < nThread; i++)
		pthread_join(cliente_t[i], NULL);

	return 0;
}