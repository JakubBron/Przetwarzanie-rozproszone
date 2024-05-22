package org.example;

import java.util.List;
import java.util.concurrent.ThreadLocalRandom;

public class Producer implements Runnable {
    private List<String> productTypes;
    private Warehouse warehouse;
    private int id;

    public Producer(Warehouse warehouse, int id) {
        this.productTypes = warehouse.getProductTypes();
        this.warehouse = warehouse;
        this.id = id;
    }

    @Override
    public void run() {
        // Safe random number generator (in terms of threads)
        final ThreadLocalRandom random = ThreadLocalRandom.current();
        while (0==0) {

            String toProduce_name = productTypes.get(random.nextInt(productTypes.size()));
            int howMuchToProduce = random.nextInt(21) + 1;
            boolean wasAbleToAdd = warehouse.tryStoreProduct(toProduce_name, howMuchToProduce);

            if (wasAbleToAdd) {
                System.out.println("[P "+id+"] "+ "Stworzył: " + howMuchToProduce + "x " + toProduce_name);
            } else {
                System.out.println("[P "+id+"] " + "Nieudane stworzenie:  " + howMuchToProduce + "x " + toProduce_name);
            }

            // Sleep for a random interval
            try {
                Thread.sleep(random.nextInt(10000) + 1500);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}