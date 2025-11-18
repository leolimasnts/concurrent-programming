import java.util.LinkedList;

//-------------------------------------------------------------------------------
/**
 * Classe FilaTarefas - Implementação de um pool de threads
 * 
 * Esta classe gerencia um conjunto fixo de threads (pool) que processam tarefas
 * de forma concorrente. As tarefas são armazenadas em uma fila (LinkedList) e
 * distribuídas para as threads disponíveis.
 * 
 * Funcionamento:
 * - Ao ser criada, inicializa um número fixo de threads (nThreads) que ficam em espera
 * - Quando uma tarefa (Runnable) é adicionada via execute(), ela é colocada na fila
 * - As threads do pool pegam tarefas da fila e as executam
 * - Se a fila está vazia, as threads ficam em wait() até que uma nova tarefa chegue
 * - O método shutdown() sinaliza o encerramento e aguarda todas as threads terminarem
 * 
 * Sincronização:
 * - A fila é protegida por sincronização (synchronized) para evitar condições de corrida
 * - notify() acorda uma thread em espera quando uma tarefa é adicionada
 * - notifyAll() acorda todas as threads quando shutdown() é chamado
 */
class FilaTarefas {
    private final int nThreads;
    private final MyPoolThreads[] threads;
    private final LinkedList<Runnable> queue;
    private boolean shutdown;

    public FilaTarefas(int nThreads) {
        this.shutdown = false;
        this.nThreads = nThreads;
        queue = new LinkedList<Runnable>();
        threads = new MyPoolThreads[nThreads];
        for (int i=0; i<nThreads; i++) {
            threads[i] = new MyPoolThreads();
            threads[i].start();
        } 
    }

    public void execute(Runnable r) {
        synchronized(queue) {
            if (this.shutdown) return;
            queue.addLast(r);
            queue.notify();
        }
    }
    
    public void shutdown() {
        synchronized(queue) {
            this.shutdown=true;
            queue.notifyAll();
        }
        for (int i=0; i<nThreads; i++) {
          try { threads[i].join(); } catch (InterruptedException e) { return; }
        }
    }

    private class MyPoolThreads extends Thread {
       public void run() {
         Runnable r;
         while (true) {
           synchronized(queue) {
             while (queue.isEmpty() && (!shutdown)) {
               try { queue.wait(); }
               catch (InterruptedException ignored){}
             }
             if (queue.isEmpty()) return;   
             r = (Runnable) queue.removeFirst();
           }
           try { r.run(); }
           catch (RuntimeException e) {}
         } 
       } 
    } 
}

class Hello implements Runnable {
   String msg;
   public Hello(String m) { msg = m; }

   public void run() {
      System.out.println(msg);
   }
}

class Primo implements Runnable {
   private long numero;
   
   public Primo(long n) { 
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

   public void run() {
      boolean resultado = ehPrimo(numero);
      if(resultado) {
         System.out.println(numero + " eh primo");
      } else {
         System.out.println(numero + " nao eh primo");
      }
   }
}

class MyPool {
    private static final int NTHREADS = 10;

    public static void main (String[] args) {

      FilaTarefas pool = new FilaTarefas(NTHREADS); 

      for (int i = 0; i < 25; i++) {
        Runnable primo = new Primo(i);
        pool.execute(primo);
      }

      pool.shutdown();
      System.out.println("Terminou");
   }
}
