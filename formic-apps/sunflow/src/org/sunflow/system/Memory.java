package org.sunflow.system;

public final class Memory {
    public static final String sizeof(int[] array) {
        return bytesToString(array == null ? 0 : 4 * array.length);
    }

    public static final String bytesToString(long bytes) {
        if (bytes < 1024)
            return bytes+"b";
        if (bytes < 1024 * 1024)
            return ((bytes + 512) >>> 10) + "Kb";
        return ((bytes + 512 * 1024) >>> 20) + "Mb";
    }
}
