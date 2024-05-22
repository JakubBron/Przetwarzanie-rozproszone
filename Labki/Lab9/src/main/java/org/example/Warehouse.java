package org.example;

import java.util.HashMap;
import java.util.List;

public class Warehouse {
    private final List<String> productTypes;
    private HashMap<String, Integer> storage = new HashMap<>();
    private int maxStorage;

    public Warehouse(List<String> productTypes, int maxStorage) {
        this.productTypes = productTypes;
        this.maxStorage = maxStorage;
    }

    public synchronized boolean tryStoreProduct(String productName, int amount) {
        if (!productTypes.contains(productName) || (storage.getOrDefault(productName, 0) + amount > maxStorage) ) {
            return false;
        }

        storage.put(productName, storage.getOrDefault(productName, 0) + amount);
        System.out.println("[W] Zmagazynowano: " + amount + "x " + productName);
        return true;
    }

    public synchronized boolean tryGetProduct(String productName, int amount) {
        if (!productTypes.contains(productName)) {
            return false;
        }

        if (storage.getOrDefault(productName, 0) == 0 || (amount > storage.getOrDefault(productName, 0)) ) {
            System.out.println("[W] Odmowa! Towaru brak lub niemozliwe do realizacji (" + amount + "x " + productName +", jest "+storage.getOrDefault(productName, 0) + ")");
            return false;
        }

        Integer present = storage.getOrDefault(productName, 0);
        storage.put(productName, 0);
        System.out.println("[W] Wyslano: " + amount + " / " + present + " sztuk " + productName);
        return true;
    }

    public List<String> getProductTypes() {
        return productTypes;
    }
}