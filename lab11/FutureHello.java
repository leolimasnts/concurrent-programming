/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 11 */
/* Codigo: Exemplo de uso de futures */
/* -------------------------------------------------------------------*/

import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import java.util.ArrayList;
import java.util.List;


class MyCallable implements Callable<Long> {
  MyCallable() {}
 
  public Long call() throws Exception {
    long s = 0;
    for (long i=1; i<=100; i++) {
      s++;
    }
    return s;
  }
}

class PrimoCallable implements Callable<Boolean> {
  private long numero;
  
  PrimoCallable(long n) {
    this.numero = n;
  }
 
  private boolean ehPrimo(long n) {
    if(n <= 1) return false;
    if(n == 2) return true;
    if(n % 2 == 0) return false;
    for(long i = 3; i <= Math.sqrt(n); i += 2) {
      if(n % i == 0) return false;
    }
    return true;
  }

  public Boolean call() throws Exception {
    return ehPrimo(numero);
  }
}

public class FutureHello  {
  private static final long N = 1000000; // Pode ser um valor bastante grande
  private static final int NTHREADS = 10;

  public static void main(String[] args) {
    long startTime = System.currentTimeMillis();
    
    ExecutorService executor = Executors.newFixedThreadPool(NTHREADS);
    List<Future<Boolean>> list = new ArrayList<Future<Boolean>>();

    for (long i = 1; i <= N; i++) {
      Callable<Boolean> worker = new PrimoCallable(i);

      Future<Boolean> submit = executor.submit(worker);
      list.add(submit);
    }

    System.out.println("Total de tarefas submetidas: " + list.size());

    long countPrimos = 0;
    for (Future<Boolean> future : list) {
      try {
        if (future.get()) { 
          countPrimos++;
        }
      } catch (InterruptedException e) {
        e.printStackTrace();
      } catch (ExecutionException e) {
        e.printStackTrace();
      }
    }
    
    long endTime = System.currentTimeMillis();
    System.out.println("Quantidade de numeros primos de 1 a " + N + ": " + countPrimos);
    System.out.println("Tempo de execucao: " + (endTime - startTime) + " ms");
    executor.shutdown();
  }
}
