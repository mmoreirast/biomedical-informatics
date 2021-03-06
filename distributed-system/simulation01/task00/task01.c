/*
*	Autor: Mariana Moreira dos Santos
*	Informatica Biomedica - GRR20186554 
*	Ultima atualização: 26 de nov, 2020. 20:45. 
*	Descrição: 
*
*/

#include <stdio.h>
#include <stdlib.h>

// Biblioteca de simulacao do MIT
#include "smpl.h"

// Eventos
#define test 1
#define fault 2
#define recovery 3

// Intervalo de teste
#define TIME_INTERVAL 30.0

// Quantidade maximo de tempo que a simulacao deve rodar
#define MAX_TIME 150.0

// Descritor do processo
typedef struct {
	// Identificador de facility do SMPL
	int id;		

	// outras variaveis locais dos processos são declaradas aqui
} TipoProcesso;

// Ponteiro
TipoProcesso *processo;

int main (int argc, char *argv[]) {
	// Variaveis
	static int N, token, event, r, i;

	// Nome da facility
	static char fa_name[5];

	// Caso o input esteja errado
	if(argc != 2) {
		puts ("O input inserido está errado. O uso corretor é o seguinte: tempo <número de processos>");
		exit(1);
	}

	// N recebe o numero de processos inserido pelo usuario
	N = atoi(argv[1]);


	printf("===========================================================\n"); 
 	printf("Algoritmo VRing\n");
  	printf("Aluna: Mariana Moreira dos Santos\n");
  	printf("Disciplina: Sistemas Distribuidos\n");
  	printf("Ultima Modificacao: 15/12/2020\n");
  	printf("===========================================================\n\n"); 

	printf("----------------------------------------------------------- \n");
	printf("Existem %d processos no sistema.\n", N);
	printf("O tempo do intervalo de testes é de %4.1f unidades de tempo.\n", TIME_INTERVAL);
	printf("O tempo máximo de execução é de %4.1f unidades de tempo.\n", MAX_TIME);
	printf("----------------------------------------------------------- \n");
	printf("\n");

	smpl(0, "Um Exemplo de Simulação");
	reset();

	// Indica que existe apenas uma thread trabalhando
	stream(1);

	// Alocacao de memoria para N processos
	processo = (TipoProcesso *) malloc(sizeof(TipoProcesso) * N);

	// Inicializando os processos
	for (i = 0; i < N; i++){
		// Copia a string para o fa_name
		memset(fa_name, '\0', 5);

		// Envia a saída formatada para uma string
		sprintf(fa_name, "%d", i);

		// Setando o ID para o SMPL
		processo[i].id = facility(fa_name, 1);
	}

	// Escalonamento inicial de eventos
	// Nossos processos vao executar testes em intervalos detestes
	// O intervalo de testes vai ser de 30 unidades de tempo
	// A simulacao começa no tempo 0 e vamos escalonar o primeiro teste 
	// de todos os processos para o tempo 30

	for (i = 0; i < N; i++){
		schedule (test, 30.0, i);
	}
	schedule (fault, 31.0, 1);
	schedule (recovery, 61.0, 1);

	// Quantidade maximo de tempo que a simulacao deve rodar eh 150
	while (time() < 150.0){
		// Retorna o ID do nodo que esta sendo executado
		// e qual evento ele esta executando
		cause(&event, &token);

		// Dependendo do evento, faca..
		switch(event) {

			// Test
			case test:
				// Se o processo esta falho, nao testa (crash-recovery)
				if (status(processo[token].id) != 0)
					break;
				printf("O processo %d vai testar no tempo %4.1f\n", token, time());
				schedule(test, 30.0, token);
				break;

			// Fault
			case fault:
				r = request (processo[token].id, token, 0);
				if (r != 0) {
					// Isso ocorre caso a gente tenha esquecido de que ele já falhou antes
					printf("Não foi possível falhar o processo %d", token);
					exit (1);
				}
				printf("O processo %d falhou no tempo %4.1f\n", token, time());
				break;

			// Recovery
			case recovery:
				release(processo[token].id, token);
				printf ("O processo %d recuperou no tempo %4.1f\n", token, time());
				schedule (test, 30.0, token);
				break;
		}
	}
}