/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 11 */
/* Codigo: Exemplo de uso de um pool de threads oferecido por Java */
/* -------------------------------------------------------------------*/

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicLong;

class Worker implements Runnable {
  private final long steps;
  private static AtomicLong sharedCounter = new AtomicLong(0);

  Worker(long numSteps) {
    this.steps = numSteps;
  }

  public void run() {
    long s = 0;
    for (long i=1; i<this.steps; i++) {
      s += i;
    }
    System.out.println(s);
    sharedCounter.incrementAndGet();
  }

  public static long getSharedCounter() {
    return sharedCounter.get();
  }
}

public class AnotherHelloPool {
  private static final int NTHREADS = 10;
  private static final int WORKERS = 50;

  public static void main(String[] args) {
    ExecutorService executor = Executors.newFixedThreadPool(NTHREADS);

    for (int i = 1; i < WORKERS; i++) {
      Runnable worker = new Worker(i);
      executor.execute(worker);
    }

    executor.shutdown();
    
    while (!executor.isTerminated()) {}
    System.out.println("Terminou");
    System.out.println("Valor final da variavel compartilhada: " + Worker.getSharedCounter());
   }
}
