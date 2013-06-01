/*
 * Copyright (c) 2008 Standard Performance Evaluation Corporation (SPEC)
 *               All rights reserved.
 *
 * This source code is provided as is, without any express or implied warranty.
 */
package spec.reporter;


public class BenchmarkChart {
    public BenchmarkChart(String bname) {
    }

    public void setStartTime(long startTime) {
    }

    public void addTotalHeapInfo(long time, long bytes) {
    }


    public void addFreeHeapInfo(long time, long bytes) {
    }

    public void addAnalyzerReport(String name, String unit, long time, long value) {
    }

    public void addTimeInfo(String threadName, long startTime, long endTime) {
    }

    public void addTimeInfo(long startTime, long endTime) {
    }

    public void addMarker(long time, String label) {
    }

    private void setMemoryAxis() {
    }


    public void buildJPEG() {
    }

    private void updateColors() {
    }
}
