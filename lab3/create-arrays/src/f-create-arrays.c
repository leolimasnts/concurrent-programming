#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fputs("  Incorrect number of parameters! :(\n", stderr);
    fprintf(stderr, "use %s <dimension> <file name>\n", argv[0]);
    exit(1);
  }

  srand(time(NULL));
  FILE *file;
  file = fopen(argv[2], "wb");
  long int dimension = atol(argv[1]);

  float A[dimension];
  float B[dimension];

  for (int i = 0; i < dimension; i++) {
    A[i] = (float)rand() / RAND_MAX; // random float [0, 1]
    B[i] = (float)rand() / RAND_MAX; // random float [0, 1]
    // A[i] = (float)(rand() % 1000) / 3;
    // B[i] = (float)(rand() % 1000) / 3;
  }

  float a_dot_b = 0;

  for (int i = 0; i < dimension; i++) {
    a_dot_b += A[i] * B[i];
  }

  fwrite(&dimension, sizeof(long int), 1, file);
  fwrite(A, sizeof(float), dimension, file);
  fwrite(B, sizeof(float), dimension, file);
  fwrite(&a_dot_b, sizeof(float), 1, file);

  printf("file %s.bin created succefully! :)", argv[2]);
  fclose(file);
}
