#include "timer.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

float *A;
float *B;

typedef struct {
  int start;
  int length;
} task;

float *new_array(int length) {
  float *arr = (float *)malloc(sizeof(float) * length);
  if (arr == NULL) {
    fputs("  Error: can not alocate array", stderr);
  }

  for (int i = 0; i < length; i++) {
    arr[i] = i;
  }
  return arr;
}

float operate(int i) { return A[i] * B[i]; }

void *thread_main(void *arg) {

  // typecast to the correct type and make a copy
  task t = *(task *)arg;
  float *tmp = (float *)malloc(sizeof(float));
  float a_dot_b = 0;
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
  float expected_output;

  FILE *file = fopen(argv[2], "rb");
  fread(&array_length, sizeof(long int), 1, file);

  A = new_array(array_length);
  B = new_array(array_length);

  fread(A, sizeof(float), array_length, file);
  fread(B, sizeof(float), array_length, file);
  fread(&expected_output, sizeof(float), 1, file);

  // variables to measure time
  double start_concurrent;
  double end_concurrent;
  double elapsed_concurrent;

  GET_TIME(start_concurrent);

  for (int i = 0; i < number_of_threads; i++) {
    task *t = malloc(sizeof(task));

    t->length = array_length / number_of_threads;
    t->start = i * t->length;

    if (i == number_of_threads - 1) {
      t->length += array_length % number_of_threads;
    }
    pthread_create(&thread_ids[i], NULL, thread_main, (void *)t);
  }

  float *tmp;
  float a_dot_b_concurrent = 0;
  for (int i = 0; i < number_of_threads; i++) {

    pthread_join(thread_ids[i], (void **)&tmp);
    printf("Thread %d -> %f \n", i + 1, *tmp);
    a_dot_b_concurrent += *tmp;
  }

  free(tmp);

  GET_TIME(end_concurrent);
  elapsed_concurrent = end_concurrent - start_concurrent;

  double start_sequential;
  double end_sequential;
  double elapsed_sequential;

  GET_TIME(start_sequential);

  float a_dot_b_sequential = 0;
  for (int i = 0; i < array_length; i++) {
    a_dot_b_sequential += A[i] * B[i];
  }
  GET_TIME(end_sequential);
  elapsed_sequential = end_sequential - start_sequential;

  float relative_variation =
      (a_dot_b_concurrent - a_dot_b_sequential) / a_dot_b_concurrent;

  printf("\n");
  // printf("expected output:  %f\n", expected_output);

  // printf("\n"); // ---

  printf("sequential:       %f\n", a_dot_b_sequential);
  printf("took:             %fs\n", elapsed_sequential);

  printf("\n"); // ---

  printf("concurrent:       %f\n", a_dot_b_concurrent);
  printf("took:             %fs\n", elapsed_concurrent);

  printf("\n"); // ---

  printf("variation:        %f\n", relative_variation);

  free(A);
  free(B);
  return 0;
}
