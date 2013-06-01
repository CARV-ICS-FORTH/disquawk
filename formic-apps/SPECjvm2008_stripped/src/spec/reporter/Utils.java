/*
 * Copyright (c) 2008 Standard Performance Evaluation Corporation (SPEC)
 *               All rights reserved.
 *
 * This source code is provided as is, without any express or implied warranty.
 */
package spec.reporter;

import java.text.DecimalFormat;
import java.util.TreeMap;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Iterator;


import spec.harness.Constants;

public class Utils {
	public static final Double INVALID_SCORE = Double.valueOf(-1);
    static final String HTML_WRITER_TYPE = "html";
    static final String PLAIN_WRITER_TYPE = "plain";
    static final String CSCORE_NAME = "composite score";

    //SUMMARY PROPS
    static final String[] DETAILS_TABLE_COLUMNS_NAMES = new String[]{
        "benchmark name",
        "configuration",
        "iteration",
        "expected run time (millis)",
        "actual run time(millis)",
        "operations",
        Constants.WORKLOAD_METRIC,
        "graph"
    };
    static final String[] DETAILS_TABLE_BM_CONF_NAMES = new String[]{
        "number of threads",
        "minIter",
        "maxIter",
        "warmup time",
        "run time",
        "iteration delay",
        "analyzers",
        "analyzers frequency"
    };
    static final String[] BM_CONFIGURATION_ENAMES = new String[]{
        Constants.NUM_BM_THREADS_ENAME,
        Constants.MIN_ITER_ENAME,
        Constants.MAX_ITER_ENAME,
        Constants.WARMUP_TIME_ENAME,
        Constants.ITERATION_TIME_ENAME,
        Constants.ITER_DELAY_ENAME,
        Constants.ANALYZERS_ENAME,
        Constants.ANALYZER_FREQ_ENAME
    };

    //directories for reporter
    public static String REPORTER_DIR;
    public static String IMAGES_DIR;
    public static boolean debug = false;

    public static void createBmResultGraph(BenchmarkRecord record) {
    }
    private static Hashtable<String, String> descriptions;
    final static DecimalFormat df = new DecimalFormat("#.##");

    private static void initDescriptions() {
        descriptions = new Hashtable<String, String>();
        //run info
        descriptions.put(Constants.REPORTER_RUN_DATE, "Test date:");
        descriptions.put(Constants.REPORTER_RUN_SUBMITTER_URL, "Submitter URL");
        descriptions.put(Constants.REPORTER_RUN_LICENSE, "SPEC license");
        descriptions.put(Constants.REPORTER_RUN_TESTER, "Tester");
        descriptions.put(Constants.REPORTER_RUN_SUBMITTER, "Submitter");
        descriptions.put(Constants.REPORTER_RUN_LOCATION, "Location");

        //jvm info
        descriptions.put(Constants.REPORTER_JVM_VENDOR, "Vendor");
        descriptions.put(Constants.REPORTER_JVM_VENDOR_URL, "Vendor URL");
        descriptions.put(Constants.REPORTER_JVM_NAME, "JVM name");
        descriptions.put(Constants.REPORTER_JVM_VERSION, "JVM version");
        descriptions.put(Constants.REPORTER_JVM_AVAILABLE_DATE, "JVM available");
        descriptions.put(Constants.REPORTER_JVM_JAVA_SPECIFICATION, "Java Specification");
        descriptions.put(Constants.REPORTER_JVM_ADDRESS_BITS, "JVM address bits");
        descriptions.put(Constants.REPORTER_JVM_COMMAND_LINE_INITIAL_HEAP_SIZE, "JVM initial heap memory");
        descriptions.put(Constants.REPORTER_JVM_COMMAND_LINE_MAX_HEAP_SIZE, "JVM maximum heap memory");
        descriptions.put(Constants.REPORTER_JVM_COMMAND_LINE, "JVM command line");
        descriptions.put(Constants.REPORTER_JVM_STARTUP_COMMAND_LINE, "JVM command line startup");
        descriptions.put(Constants.REPORTER_JVM_STARTUP_LAUNCHER, "JVM launcher startup");
        descriptions.put(Constants.REPORTER_JVM_OTHER_TUNING, "Additional JVM tuning");
        descriptions.put(Constants.REPORTER_JVM_APP_CLASS_PATH, "JVM class path");
        descriptions.put(Constants.REPORTER_JVM_BOOT_CLASS_PATH, "JVM boot class path");

        //sw-info
        descriptions.put(Constants.REPORTER_OS_NAME, "OS name");
        descriptions.put(Constants.REPORTER_OS_AVAILABLE_DATE, "OS available");
        descriptions.put(Constants.REPORTER_OS_ADDRESS_BITS, "OS address bits");
        descriptions.put(Constants.REPORTER_OS_TUNING, "OS tuning");
        descriptions.put(Constants.REPORTER_SW_FILESYSTEM, "Filesystem");
        descriptions.put(Constants.REPORTER_SW_OTHER_NAME, "Other s/w name");
        descriptions.put(Constants.REPORTER_SW_OTHER_TUNING, "Other s/w tuning");
        descriptions.put(Constants.REPORTER_SW_OTHER_AVAILABLE, "Other s/w available");

        //hw-info
        descriptions.put(Constants.REPORTER_HW_VENDOR, "HW vendor");
        descriptions.put(Constants.REPORTER_HW_VENDOR_URL, "HW vendor's URL");
        descriptions.put(Constants.REPORTER_HW_MODEL, "HW model");
        descriptions.put(Constants.REPORTER_HW_AVAILABLE, "HW available");
        descriptions.put(Constants.REPORTER_HW_CPU_VENDOR, "CPU vendor");
        descriptions.put(Constants.REPORTER_HW_CPU_VENDOR_URL, "CPU vendor's URL");
        descriptions.put(Constants.REPORTER_HW_CPU_NAME, "CPU name");
        descriptions.put(Constants.REPORTER_HW_CPU_SPEED, "CPU frequency");
        descriptions.put(Constants.REPORTER_HW_LOGICAL_CPUS, "# of logical cpus");
        descriptions.put(Constants.REPORTER_HW_NUMBER_OF_CHIPS, "# of chips");
        descriptions.put(Constants.REPORTER_HW_NUMBER_OF_CORES, "# of cores");
        descriptions.put(Constants.REPORTER_HW_NUMBER_OF_CORES_PER_CHIP, "Cores per chip");
        descriptions.put(Constants.REPORTER_HW_THREADS_PER_CORE, "Threads per core");
        descriptions.put(Constants.REPORTER_HW_THREADING_ENABLED, "Threading enabled");
        descriptions.put(Constants.REPORTER_HW_ADDRESS_BITS, "HW address bits");
        descriptions.put(Constants.REPORTER_HW_CPU_CACHE_L1, "Primary cache");
        descriptions.put(Constants.REPORTER_HW_CPU_CACHE_L2, "Secondary cache");
        descriptions.put(Constants.REPORTER_HW_CPU_CACHE_OTHER, "Other cache");
        descriptions.put(Constants.REPORTER_HW_MEMORY_DETAILS, "Memory details");
        descriptions.put(Constants.REPORTER_HW_MEMORY_SIZE, "Memory size");
        descriptions.put(Constants.REPORTER_HW_DETAILS_OTHER, "Other HW details");

        descriptions.put("category", "Category of run");
        descriptions.put("iterationTime", "Iteration duration");
        descriptions.put("warmupTime", "Warm up time");
        descriptions.put("minIter", "Minimum Iterations");
        descriptions.put("maxIter", "Maximum Iterations");
        descriptions.put("analyzers", "Analyzer names");
        descriptions.put("analyzerFreq", "Analyzer frequency");
        descriptions.put("specjvm.benchmark.systemgc", "System.gc in between benchmarks");
        descriptions.put("benchmarkDelay", "Benchmark delay");
        descriptions.put("specjvm.iteration.systemgc", "System.gc in between iterations");
        descriptions.put("iterationDelay", "Iteration delay");
    }

    public static String getDescription(String prop) {
        if (descriptions == null) {
            initDescriptions();
        }
        Object result = descriptions.get(prop);
        return (result != null) ? (String) result : prop;
    }

    public static void generateMainChart(double compositeScore,
                                         TreeMap<String, Double> scores) {
    }

    static final boolean isCheck(BenchmarkRecord record) {
        return Constants.CHECK_BNAME.equals(record.name);
    }

    static final boolean isScimarkMonteCarlo(BenchmarkRecord record) {
        return record != null && Constants.SCIMARK_MONTE_CARLO_BNAME.equals(record.name);
    }

    static final boolean isScimarkLarge(BenchmarkRecord record) {
        return record.name.startsWith(Constants.SCIMARK_BNAME_PREFIX) && record.name.endsWith(Constants.SCIMARK_BNAME_LARGE_POSTFIX);
    }

    static final boolean isScimarkSmall(BenchmarkRecord record) {
        return record.name.startsWith(Constants.SCIMARK_BNAME_PREFIX) && record.name.endsWith(Constants.SCIMARK_BNAME_SMALL_POSTFIX);
    }

    static final String getImageName(String pictureName) {
        return "images/" + pictureName + ".jpg";
    }

    static final String getFullImageName(String pictureName) {
        return IMAGES_DIR + "/" + pictureName + ".jpg";
    }

    static final String getRunStatus(boolean validRun,
            boolean compliantRun) {
        if (validRun && compliantRun) {
            return "Run is compliant";
        } else if (validRun) {
            return "Run is valid, but not compliant";
        } else {
            return "Run is not valid";
        }
    }

    static final String formatScore(double score) {
    	return formatScore(score, "", "");
    }

    static final String formatScore(double score, String postfix) {
    	return Utils.formatScore(score, "", postfix);
    }

    static final String formatScore(double score, String prefix, String postfix) {
    	return !Utils.isValidScore(score)  ? "not valid" : prefix + df.format(score) + postfix;
    }

    static void dbgPrint(String str) {
        if (debug) {
            System.out.println(str);
        }
    }

    static boolean isValidScore(Double score) {
    	return !INVALID_SCORE.equals(score);
    }

    static boolean isValidScore(double score) {
    	return !INVALID_SCORE.equals(Double.valueOf(score));
    }
}
