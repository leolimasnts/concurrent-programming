#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

int N;
int M;
int V;

long long int *buffer;
int next_producer_position = 0;
int next_consumer_position = 0;

sem_t mutex_buffer;
sem_t full_positions;
sem_t empty_positions;
sem_t mutex_result;

int number_of_processed = 0;
int total_primes = 0;

int *primes_per_consumer;

int is_prime(long long int n) {
  int i;
  if (n <= 1)
    return 0;
  if (n == 2)
    return 1;
  if (n % 2 == 0)
    return 0;
  for (int i = 3; i < sqrt(n) + 1; i += 2) {
    if (n % i == 0)
      return 0;
  }
  return 1;
}
void *consumer(void *arg) {
  long long int value;
  int id = (int)(long)arg;
  int number_of_primes_found = 0;

  while (1) {
    sem_wait(&full_positions);

    sem_wait(&mutex_buffer);

    value = buffer[next_consumer_position];
    next_consumer_position = (next_consumer_position + 1) % M;

    sem_post(&mutex_buffer);
    sem_post(&empty_positions);

    if (value == -1) {
      for (int i = 0; i < V; i++)
        sem_post(&full_positions);
      break;
    }

    if (value >= 0) {

      if (is_prime(value))
        number_of_primes_found++;

      sem_wait(&mutex_result);
      number_of_processed++;
      total_primes += is_prime(value);
      sem_post(&mutex_result);
    }

    primes_per_consumer[id] = number_of_primes_found;
  }
  pthread_exit(NULL);
}

void *producer(void *arg) {
  long long int actual_number = 0;
  int is_finished = 0;

  while (!is_finished) {

    for (int i = 0; i < M; i++) {
      sem_wait(&empty_positions);
    }

    sem_wait(&mutex_buffer);

    for (int i = 0; i < M; i++) {
      if (actual_number < N) {
        buffer[next_producer_position] = actual_number;
        actual_number++;
      } else if (is_finished < V) {
        buffer[next_producer_position] = -1;
        is_finished++;
      } else {
        buffer[next_producer_position] = 0;
      }
      next_producer_position = (next_producer_position + 1) % M;
    }

    sem_post(&mutex_buffer);

    for (int i = 0; i < M; i++)
      sem_post(&full_positions);

    if (is_finished == V)
      break;
  }
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    fputs("  Incorrect number of arguments\n", stderr);
    fprintf(stderr, "Run: %s <total> <buffer size> <number of consumers>\n",
            argv[0]);
    exit(1);
  }
  N = atoi(argv[1]);
  M = atoi(argv[2]);
  V = atoi(argv[3]);

  buffer = (long long int *)malloc(sizeof(long long int) * M);
  primes_per_consumer = (int *)calloc(V, sizeof(int));
  pthread_t *consumer_tids = (pthread_t *)malloc(sizeof(pthread_t) * V);

  if (!buffer || !primes_per_consumer || !consumer_tids) {
    fprintf(stderr, "Error allocating the resources\n");
    exit(1);
  }

  sem_init(&mutex_buffer, 0, 1);
  sem_init(&mutex_result, 0, 1);

  sem_init(&full_positions, 0, 0);
  sem_init(&empty_positions, 0, M);

  pthread_t producer_tid;
  pthread_create(&producer_tid, NULL, producer, NULL);
  for (int i = 0; i < V; i++) {
    pthread_create(&consumer_tids[i], NULL, consumer, (void *)(long)i);
  }

  pthread_join(producer_tid, NULL);
  for (int i = 0; i < V; i++) {
    pthread_join(consumer_tids[i], NULL);
  }

  int max_primes = -1;
  int winner_id;

  for (int i = 0; i < V; i++) {
    if (primes_per_consumer[i] > max_primes) {
      max_primes = primes_per_consumer[i];
      winner_id = i;
    }
  }

  printf("winner: %d\nfound %d primes\n\n", winner_id, max_primes);
  printf("total number of primes: %d", total_primes);

  sem_destroy(&mutex_buffer);
  sem_destroy(&mutex_result);
  sem_destroy(&full_positions);
  sem_destroy(&empty_positions);
  free(buffer);
  free(primes_per_consumer);
  free(consumer_tids);

  return 0;
}
