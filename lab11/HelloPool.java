import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

class Hello implements Runnable {
   String msg;
   public Hello(String m) { msg = m; }

   public void run() {
      System.out.println(msg); 
   }
}

class HelloPool {
   private static final int NTHREADS = 10;

   public static void main(String[] args) {

      ExecutorService pool = Executors.newFixedThreadPool(NTHREADS);

      for (int i = 0; i < 25; i++) {
        final String m = "Hello da tarefa " + i;
        Runnable r = new Hello(m);
        pool.execute(r);
      }

      pool.shutdown();
      while (!pool.isTerminated()) {}
      System.out.println("Terminou");
   }
}
