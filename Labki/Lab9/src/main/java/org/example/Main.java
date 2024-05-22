package org.example;

import java.io.IOException;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Main {
    private static List<String> productTypes = List.of("Jablko", "Ananas", "Wisnia", "Malina");

    public static void main(String[] args) {
        Warehouse warehouse = new Warehouse(productTypes, 210);

        int howMuchProducers = 4;
        int howMuchConsumers = 4;

        ExecutorService taskExecutor_Producers = Executors.newFixedThreadPool(howMuchProducers);
        ExecutorService taskExecuror_Consumers = Executors.newFixedThreadPool(howMuchConsumers);

        for (int i = 0; i < howMuchProducers; i++) {
            taskExecutor_Producers.execute(new Producer(warehouse, i));
        }

        for (int i = 0; i < howMuchConsumers; i++) {
            taskExecuror_Consumers.execute(new Consumer(warehouse, i));
        }

        // If Enter (10) is pressed, shutdown app and threads
        try {
            if (System.in.read() == 10) { // Enter key
                taskExecutor_Producers.shutdown();
                taskExecuror_Consumers.shutdown();
                System.exit(0);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}