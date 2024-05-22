package org.example;

import java.util.List;
import java.util.concurrent.ThreadLocalRandom;

public class Consumer implements Runnable {
    private List<String> productTypes;
    private Warehouse warehouse;
    private int id;

    public Consumer(Warehouse warehouse, int id) {
        this.productTypes = warehouse.getProductTypes();
        this.warehouse = warehouse;
        this.id = id;
    }

    @Override
    public void run() {
        // safe random number generator (in terms of threads)
        final ThreadLocalRandom random = ThreadLocalRandom.current();
        while (0==0) {

            String productToConsume_name = productTypes.get(random.nextInt(productTypes.size()));
            int howMuchToConsume = random.nextInt(10) + 1;
            boolean wasAbleToRemove = warehouse.tryGetProduct(productToConsume_name, howMuchToConsume);

            if (wasAbleToRemove) {
                System.out.println("[K "+id+"] " + "Zabrano: " + howMuchToConsume + "x " + productToConsume_name);
            } else {
                System.out.println("[K "+id+"] " + "Odmowa zabrania: " + howMuchToConsume + "x " + productToConsume_name);
            }

            try {
                Thread.sleep(random.nextInt(5000) + 2500);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }
    }
}