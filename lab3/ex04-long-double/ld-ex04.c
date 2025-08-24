#include "timer.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

long double *A;
long double *B;

typedef struct {
  int start;
  int length;
} task;

long double *new_array(int length) {
  long double *arr = (long double *)malloc(sizeof(long double) * length);
  if (arr == NULL) {
    fputs("  Error: can not alocate array", stderr);
  }

  for (int i = 0; i < length; i++) {
    arr[i] = i;
  }
  return arr;
}

long double operate(int i) { return A[i] * B[i]; }

void *thread_main(void *arg) {

  // typecast to the correct type and make a copy
  task t = *(task *)arg;
  long double *tmp = (long double *)malloc(sizeof(long double));
  long double a_dot_b = 0;
  // deletes the original
  free(arg);

  for (int i = t.start; i < (t.start + t.length); i++) {
    a_dot_b += operate(i);
  }
  *tmp = a_dot_b;
  pthread_exit(tmp);
}

void array_print(int *v, int n) {
  printf("[ ");

  for (int i = 0; i < n; i++) {
    printf("%d ", v[i]);
  }
  printf("]\n");
}

int main(int argc, char **argv) {

  // checks arguments correctness
  if (argc != 3) {
    fputs("  Incorrect number of arguments\n", stderr);
    fprintf(stderr, "Run: %s <number of threads> <file name>\n", argv[0]);
    exit(1);
  }

  int number_of_threads = atoi(argv[1]);
  long int array_length;
  pthread_t thread_ids[number_of_threads];
  long double expected_output;

  FILE *file = fopen(argv[2], "rb");
  fread(&array_length, sizeof(long int), 1, file);

  A = new_array(array_length);
  B = new_array(array_length);

  fread(A, sizeof(long double), array_length, file);
  fread(B, sizeof(long double), array_length, file);
  fread(&expected_output, sizeof(long double), 1, file);

  // variables to measure time
  double start;
  double end;
  double elapsed;

  GET_TIME(start);

  for (int i = 0; i < number_of_threads; i++) {
    task *t = malloc(sizeof(task));

    t->length = array_length / number_of_threads;
    t->start = i * t->length;

    if (i == number_of_threads - 1) {
      t->length += array_length % number_of_threads;
    }
    pthread_create(&thread_ids[i], NULL, thread_main, (void *)t);
  }

  long double *tmp;
  long double a_dot_b = 0;
  for (int i = 0; i < number_of_threads; i++) {

    pthread_join(thread_ids[i], (void **)&tmp);
    printf("Thread %d -> %Lf \n", i + 1, *tmp);
    a_dot_b += *tmp;
  }

  free(tmp);

  GET_TIME(end);
  elapsed = end - start;

  printf("\ntook:          %fs\n", elapsed);
  printf("concurrent:    %Lf\n", a_dot_b);
  printf("sequential:    %Lf\n", expected_output);

  free(A);
  free(B);
  return 0;
}
