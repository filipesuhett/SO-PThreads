#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#pragma comment(lib, "pthreadVC2.lib")
#define HAVE_STRUCT_TIMESPEC

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

pthread_mutex_t mutex_Macroblocos;
pthread_mutex_t mutex_Soma;

#define NUM_THREADS 8

#define linMatriz 10000
#define colMatriz 10000

#define linMacro 1000
#define colMacro 1000

int qtdMacroblocos = (linMatriz*colMatriz)/(linMacro*colMacro);

int numMacroblocos = 1;

int primos = 0;

double tempoParalelo;

double tempoParaleloMutex;

double tempoSerial;

int ** matriz;

void prints() {
    printf("BEM-VINDO GIRAS - PTHREADS\n");
    printf("\n");
    printf("AUTORES: \n");
    printf("Giovanna Scalfoni - VULGO: GIGI\n");
    printf("Filipe Suhett - VULGO: SHUBERT\n");
    printf("\n");
    printf("Inicializando a Matriz...\n");
    printf("\n");
    printf("Dados Gerais:\n");
    printf("Tamanho da Matriz: %d x %d\n", linMatriz, colMatriz);
    printf("Quantidade de macroblocos: %d\n", qtdMacroblocos);
    printf("Tamanho do Macrobloco: %d x %d\n", linMacro, colMacro);
    printf("Quantidade de Threads: %d\n", NUM_THREADS);
    printf("\n");
    printf("ESPERO QUE SE DIVIRTA TESTANDO NOSSO PROGRAMA!!! Te amamos S2\n");
    printf("\n");
}   

void inicializarMatriz() {
    prints();

    matriz = (int**)malloc(linMatriz * sizeof(int*));
    if (matriz == NULL) {
        printf("** Erro: Memoria Insuficiente **");
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < linMatriz; i++) {
        matriz[i] = (int*)malloc(colMatriz * sizeof(int));
        if (matriz[i] == NULL) {
            printf("** Erro: Memoria Insuficiente **");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < linMatriz; i++) {
        for (int j = 0; j < colMatriz; j++) {
            matriz[i][j] = (rand() % 32000);
        }
    }
}

int ehPrimo(int numero) {
    if (numero < 2) {
        return 0;
    }
    double raiz = sqrt(numero);
    for (int i = 2; i <= raiz; i++) {
        if (numero % i == 0) {
            return 0;
        }
    }
    return 1;
}

void* verificaPrimoSerial(void* arg) {
    printf("Verificando numeros primos de forma SERIAL...\n");
    for (int i = 0; i < linMatriz; i++) {
        for (int j = 0; j < colMatriz; j++) {
            primos += ehPrimo(matriz[i][j]);
        }
    }
    printf("Total de numeros primos: %d - SERIAL\n", primos);
}

void primoSerial() {
    clock_t inicio = clock();
    verificaPrimoSerial(NULL);
    clock_t fim = clock();
    tempoSerial = (double)(fim - inicio) / CLOCKS_PER_SEC;
    printf("Tempo de execucao serial: %f\n", tempoSerial);
    printf("\n");
}

int calculaMacroBlocos(int n) {
    int num_primos = 0;
    
    if (linMatriz % linMacro == 0) {
        int linStart;
        int linEnd;

        if (((n * linMacro) % linMatriz) - linMacro < 0) {
            linStart = (((n * linMacro) % linMatriz) - linMacro) + linMatriz;
            linEnd = linStart + linMacro - 1;
        }

        else {
            linStart = ((n * linMacro) % linMatriz) - linMacro;
            linEnd = linStart + linMacro - 1;
        }

        int aux = floor(((n * linMacro) - 1) / linMatriz);

        int colStart = aux * colMacro;
        int colEnd = colStart + colMacro - 1;
        

        for (int i = linStart; i <= linEnd; i++) {
            for (int j = colStart; j <= colEnd; j++) {
                num_primos += ehPrimo(matriz[i][j]);
            }
        }
    }
    else {
        int colStart;
        int colEnd;

        if (((n * colMacro) % colMatriz) - colMacro < 0) {
            colStart = (((n * colMacro) % colMatriz) - colMacro) + colMatriz;
            colEnd = colStart + colMacro - 1;
        }

        else {
            colStart = ((n * colMacro) % colMatriz) - colMacro;
            colEnd = colStart + colMacro - 1;
        }

        int aux = floor(((n * colMacro) - 1) / colMatriz);

        int linStart = aux * linMacro;
        int linEnd = linStart + linMacro - 1;

        for (int i = colStart; i <= colEnd; i++) {
            for (int j = linStart; j <= linEnd; j++) {
                num_primos += ehPrimo(matriz[i][j]);
            }
        }
    }

    return num_primos;
}

void * verificaPrimoParalelo() {
    while (1) {
        pthread_mutex_lock(&mutex_Macroblocos);
        
        int num = numMacroblocos;

        numMacroblocos++;

        if (num > qtdMacroblocos) {
            num = -1;
        }
                
        pthread_mutex_unlock(&mutex_Macroblocos);


        if (num == -1) {
            break;
        }

        int num_primos = calculaMacroBlocos(num);


        pthread_mutex_lock(&mutex_Soma);

        primos += num_primos;

        pthread_mutex_unlock(&mutex_Soma);

    }
    pthread_exit(NULL);
}


void primoParalelo(){
    printf("Verificando numeros primos de forma PARALELA...\n");
    primos = 0;
    pthread_mutex_init(&mutex_Macroblocos, NULL);
    pthread_mutex_init(&mutex_Soma, NULL);
    pthread_t workers[NUM_THREADS];
    
    clock_t inicio1 = clock();

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&workers[i], NULL, verificaPrimoParalelo, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++){
        pthread_join(workers[i], NULL);
    }
    
    printf("Total de numeros primos: %d - PARALELO\n", primos);

    clock_t fim1 = clock();
    tempoParalelo = (double)(fim1 - inicio1) / CLOCKS_PER_SEC;
    printf("Tempo de execucao paralelo: %f\n", tempoParalelo);
    printf("\n");
 
    pthread_mutex_destroy(&mutex_Macroblocos);
    pthread_mutex_destroy(&mutex_Soma);


}

void liberarMatriz() {
    for (int i = 0; i < linMatriz; i++) {
        free(matriz[i]);
    }
    free(matriz);
}

int main() {
    unsigned int seed = 1;
    srand(seed);
    inicializarMatriz();
    primoSerial();
    primoParalelo();
    liberarMatriz();

    printf("Calculando Speedup...\n");
    printf("Speedup: %f\n", tempoSerial/tempoParalelo);
    return 0;
}