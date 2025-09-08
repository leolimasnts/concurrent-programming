/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Codigo: Comunicação entre threads usando variável compartilhada e exclusao
 * mutua com bloqueio */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

long int soma = 0; // variavel compartilhada entre as threads
short int printer_time = 0;
short int active_workers;
pthread_mutex_t mutex; // variavel de lock para exclusao mutua
pthread_cond_t adder;
pthread_cond_t printer;

// funcao executada pelas threads
void *ExecutaTarefa(void *arg) {
  long int id = (long int)arg;
  printf("Thread : %ld esta executando...\n", id);

  for (int i = 0; i < 100000; i++) {
    pthread_mutex_lock(&mutex);

    while (printer_time == 1) {
      pthread_cond_wait(&adder, &mutex);
    }
    soma++;

    if (soma % 10 == 0) {
      printer_time = 1;
      pthread_cond_signal(&printer);
    }

    pthread_mutex_unlock(&mutex);
  }
  active_workers--;
  printf("Thread : %ld terminou!\n", id);
  pthread_exit(NULL);
}

// funcao executada pela thread de log
void *extra(void *args) {
  printf("Extra : esta executando...\n");
  while (active_workers || printer_time) {
    pthread_mutex_lock(&mutex);

    while (printer_time == 0) {
      pthread_cond_wait(&printer, &mutex);
    }
    printf("soma = %ld\n", soma);
    printer_time = 0;
    pthread_cond_broadcast(&adder);

    pthread_mutex_unlock(&mutex);
  }
  printf("Extra : terminou!\n");
  pthread_exit(NULL);
}

// fluxo principal
int main(int argc, char *argv[]) {
  pthread_t *tid; // identificadores das threads no sistema
  int nthreads;   // qtde de threads (passada linha de comando)

  //--le e avalia os parametros de entrada
  if (argc < 2) {
    printf("Digite: %s <numero de threads>\n", argv[0]);
    return 1;
  }
  nthreads = atoi(argv[1]);
  active_workers = nthreads;

  //--aloca as estruturas
  tid = (pthread_t *)malloc(sizeof(pthread_t) * (nthreads + 1));
  if (tid == NULL) {
    puts("ERRO--malloc");
    return 2;
  }

  //--inicilaiza o mutex (lock de exclusao mutua)
  pthread_mutex_init(&mutex, NULL);

  //--cria as threads
  for (long int t = 0; t < nthreads; t++) {
    if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
      printf("--ERRO: pthread_create()\n");
      exit(-1);
    }
  }

  //--cria thread de log
  if (pthread_create(&tid[nthreads], NULL, extra, NULL)) {
    printf("--ERRO: pthread_create()\n");
    exit(-1);
  }

  //--espera todas as threads terminarem
  for (int t = 0; t < nthreads + 1; t++) {
    if (pthread_join(tid[t], NULL)) {
      printf("--ERRO: pthread_join() \n");
      exit(-1);
    }
  }

  //--finaliza o mutex
  pthread_mutex_destroy(&mutex);

  printf("Valor de 'soma' = %ld\n", soma);

  return 0;
}
