/* Programacao Concorrente e Distribuida
 * Entrega 2
 * Atividade 2 a)
 * Uso da diretiva #pragma omp critical */

#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define ALIVE 1
#define DEAD 0

/* Recebe um grid NxN.
 * Insere um glider a partir da posicao (0,0).
 * Insere um R-pentomino a partir da posicao (10, 30). */
void startGrid(int** grid, int N){
	int i, j;

	// Inicializa com zeros
	for(i=0; i<N; i++)
		for(j=0; j<N; j++)
			grid[i][j] = DEAD;

	//GLIDER
	int lin = 1, col = 1;
	grid[lin  ][col+1] = 1;
	grid[lin+1][col+2] = 1;
	grid[lin+2][col  ] = 1;
	grid[lin+2][col+1] = 1;
	grid[lin+2][col+2] = 1;

	//R-pentomino
	lin =10; col = 30;
	grid[lin  ][col+1] = 1;
	grid[lin  ][col+2] = 1;
	grid[lin+1][col  ] = 1;
	grid[lin+1][col+1] = 1;
	grid[lin+2][col+1] = 1;
}

/* Recebe um grid NxN.
 * Retorna o numero de vizinhos vivos de (i, j). */
int getNeighbors(int** grid, int i, int j, int N){
	int up, down, left, right, count;

	// Define posicoes vizinhas respeitando os
	// limites do tabuleiro
	up = i == 0 ? N-1 : i-1;
	down = i == N-1 ? 0 : i + 1;
	left = j == 0 ? N - 1: j - 1;
	right = j == N - 1 ? 0 : j+1;

	count = 0;
	count += grid[up][j];
	count += grid[up][right];
	count += grid[i][right];
	count += grid[down][right];
	count += grid[down][j];
	count += grid[down][left];
	count += grid[i][left];
	count += grid[up][left];

	return count;
}

/* Recebe um grid e um newgrid NxN.
 * Simula uma geracao atualizando as celular de newgrid. */
void simulate(int** grid, int** newgrid, int N){
	int i, j, neighbors;

#pragma omp parallel private(i, j, neighbors)
{
#pragma omp for
	for(i=0; i<N; i++){
		for(j=0; j<N; j++){
			neighbors = getNeighbors(grid, i, j, N);

			// 1) Celula viva com 2 ou 3 vizinhos deve sobreviver
			if(grid[i][j] && (neighbors == 2 || neighbors == 3))
				newgrid[i][j] = ALIVE;

			// 2) Celula morta com 3 vizinhos torna-se viva
			else if(!grid[i][j] && neighbors == 3)
				newgrid[i][j] = ALIVE;

			// 3) Qualquer outro caso
			else{
				// celulas vivas devem morrer
				// celulas mortas continuam mortas
				newgrid[i][j] = DEAD;
			}
		}
	}
}

}

/* Recebe um grid NxN.
 * retorna o numero de celulas vivas no tabuleiro. */
int countAlive(int** grid, int N){
	int i, j, count;

	count = 0;

#pragma omp parallel private(i, j) shared(count, grid)
{
#pragma omp for
	for(i = 0; i < N; i++)
		for(j = 0; j < N; j++)
#pragma omp critical
			count += grid[i][j];
}

	return count;
}

int main(int argc, char** argv){
	int N, G, T;			// parametros da simulacao
	int i, j, k;			// variaveis de controle
	int **grid, **newgrid;	// estruturas do grid
	double start, end;		// contagem de tempo

	// Numero de threads
	// verifica o parametro da linha de comando
	// pega o numero de threads a ser usado
	if(argc != 2){
		printf("Uso correto: ./atv2a <numero de threads>.\n");
		return 0;
	}
	T = atoi(argv[1]);

	// Dimensao do tabuleiro NxN
	N = 2048;
	// Numero de geracoes
	G = 2000;

	// Define o numero de threads
	omp_set_num_threads(T);

	// Alocacao das matrizes NxN
	grid = (int**) malloc(N * sizeof(int*));
	newgrid = (int**) malloc(N * sizeof(int*));
	for(i=0; i<N; i++){
		grid[i] = (int*) malloc(N * sizeof(int));
		newgrid[i] = (int*) malloc(N * sizeof(int));
	}

	// Configuracao inicial do tabuleiro
	startGrid(grid, N);

	printf("\nIniciando simulacao...\n\n");
	
	// Realiza a simulacao para G geracoes
	// Alterna a posicao de grid e newgrid nas chamadas
	// (G impar -> resposta final em newgrid)
	// (G par -> resposta final em grid)
	for(i=0; i<G; i++){
		if(i % 2 == 0){
			simulate(grid, newgrid, N);
		}
		else{
			simulate(newgrid, grid, N);
		}
	}
	start = omp_get_wtime();
	if(i % 2 == 0)
		printf("Geracao %d: %d vivos.\n", i, countAlive(grid, N));
	else
		printf("Geracao %d: %d vivos.\n", i, countAlive(newgrid, N));
	end = omp_get_wtime();

	printf("\nSimulacao Finalizada.\n\n");

	// Liberacao das matrizes NxN
	for(i=0; i<N; i++){
		free(grid[i]);
		free(newgrid[i]);
	}
	free(grid);
	free(newgrid);

	printf("Tempo para contagem de vivos: %.14lf segundos.\n", end-start);

	return 0;
}
