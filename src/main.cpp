#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <thread>
#include <chrono>

#include "lru.h"
#include "fifo.h"
#include "lfu.h"
#include "arc.h"
#include "ramp.h"

#define RESET   "\033[0m"
#define BOLD    "\033[1m"

#define CYAN    "\033[36m"
#define BLUE    "\033[34m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"
#define MAGENTA "\033[35m"
#define WHITE   "\033[37m"
#define GRAY    "\033[90m"

#define CLEAR_SCREEN "\033[2J\033[H"


// ============================================================
// Timing helpers
// ============================================================

void sleepMs(int milliseconds) {

    std::this_thread::sleep_for(
        std::chrono::milliseconds(milliseconds)
    );
}


void loadingDots(
    const std::string& message,
    int delay = 500
) {

    std::cout << message << std::flush;

    for (int i = 0; i < 3; i++) {

        sleepMs(delay);

        std::cout << '.'
                  << std::flush;
    }

    std::cout << '\n';
}


void progressBar(
    const std::string& label,
    int steps = 20,
    int delay = 80
) {

    std::cout
        << label
        << " ["
        << std::flush;

    for (int i = 0; i < steps; i++) {

        std::cout
            << '#'
            << std::flush;

        sleepMs(delay);
    }

    std::cout
        << "] "
        << GREEN
        << "DONE"
        << RESET
        << '\n';
}


// ============================================================
// Result structure
// ============================================================

struct Result {

    std::string workload;
    std::string algorithm;

    int accesses;
    int hits;
    int misses;
    int evictions;

    double hitRate;

    // Actual time spent executing the cache policy.
    double executionTimeMs;
};


// ============================================================
// Workload structure
// ============================================================

struct Workload {

    std::string name;
    std::string file;
};


// ============================================================
// Run an algorithm
// ============================================================

template <typename CacheType>

Result runAlgorithm(
    const std::string& workload,
    const std::string& algorithm,
    const std::string& filename,
    int cacheSize
) {

    CacheType cache(cacheSize);

    std::ifstream file(filename);

    if (!file.is_open()) {

        throw std::runtime_error(
            "Could not open workload: " + filename
        );
    }

    int blockId;

    // Measure only the cache simulation itself.
    // File opening/reading and result processing are
    // deliberately outside the timed region.
    auto startTime =
        std::chrono::steady_clock::now();

    while (file >> blockId) {
        cache.access(blockId);
    }

    auto endTime =
        std::chrono::steady_clock::now();

    file.close();

    double executionTimeMs =
        std::chrono::duration<double, std::milli>(
            endTime - startTime
        ).count();

    Result result;

    result.workload = workload;
    result.algorithm = algorithm;

    result.accesses = cache.getAccesses();
    result.hits = cache.getHits();
    result.misses = cache.getMisses();
    result.evictions = cache.getEvictions();
    result.hitRate = cache.getHitRate();
    result.executionTimeMs = executionTimeMs;

    return result;
}


// ============================================================
// Header
// ============================================================

void printHeader() {

    std::cout << CLEAR_SCREEN;

    std::cout
        << CYAN
        << BOLD
        << "\n"
        << "============================================================\n"
        << "                 CACHE REPLACEMENT LAB\n"
        << "============================================================\n"
        << RESET;

    std::cout
        << GRAY
        << "        Comparative Cache Replacement Simulator\n"
        << RESET;

    std::cout << '\n';
}


// ============================================================
// Configuration
// ============================================================

void printConfiguration(
    int cacheSize,
    int workloadCount
) {

    std::cout
        << BOLD
        << WHITE
        << "SIMULATION CONFIGURATION\n"
        << RESET;

    std::cout
        << GRAY
        << "------------------------------------------------------------\n"
        << RESET;

    std::cout
        << "Cache Capacity : "
        << YELLOW
        << cacheSize
        << RESET
        << " blocks\n";

    std::cout
        << "Workloads      : "
        << YELLOW
        << workloadCount
        << RESET
        << '\n';

    std::cout
        << "Policies       : "
        << YELLOW
        << "LRU, FIFO, LFU, ARC, RAMP"
        << RESET
        << '\n';

    std::cout << '\n';
}


// ============================================================
// Results table
// ============================================================

void printResultsTable(
    const std::vector<Result>& results
) {

    std::cout << '\n';

    loadingDots(
        "  Preparing result matrix",
        300
    );

    std::cout << '\n';

    std::cout
        << BOLD
        << WHITE
        << "SIMULATION RESULTS\n"
        << RESET;

    std::cout
        << GRAY
        << "--------------------------------------------------------------------------\n"
        << RESET;

    std::cout
        << BOLD
        << std::left
        << std::setw(15) << "Workload"
        << std::setw(10) << "Policy"
        << std::right
        << std::setw(10) << "Hits"
        << std::setw(10) << "Misses"
        << std::setw(12) << "Evictions"
        << std::setw(12) << "Hit Rate"
        << std::setw(15) << "Time (ms)"
        << RESET
        << '\n';

    std::cout
        << GRAY
        << "--------------------------------------------------------------------------\n"
        << RESET;


    for (const auto& result : results) {

        std::string rateColor;

        if (result.hitRate >= 60.0) {
            rateColor = GREEN;
        }
        else if (result.hitRate >= 40.0) {
            rateColor = YELLOW;
        }
        else {
            rateColor = RED;
        }


        std::string policyColor = WHITE;

        if (result.algorithm == "RAMP") {
            policyColor = MAGENTA;
        }
        else if (result.algorithm == "ARC") {
            policyColor = CYAN;
        }


        std::cout
            << std::left
            << std::setw(15)
            << result.workload

            << policyColor
            << std::setw(10)
            << result.algorithm

            << RESET
            << std::right

            << std::setw(10)
            << result.hits

            << std::setw(10)
            << result.misses

            << std::setw(12)
            << result.evictions

            << rateColor
            << std::setw(11)
            << std::fixed
            << std::setprecision(2)
            << result.hitRate
            << "%"

            << RESET

            << std::setw(15)
            << std::fixed
            << std::setprecision(4)
            << result.executionTimeMs

            << '\n';

        sleepMs(80);
    }

    std::cout
        << GRAY
        << "--------------------------------------------------------------------------\n"
        << RESET;
}


// ============================================================
// Find best policy
// ============================================================


Result findBest(
    const std::vector<Result>& results,
    const std::string& workload
) {

    Result best{};

    bool found = false;

    for (const auto& result : results) {

        if (result.workload != workload) {
            continue;
        }

        // Normal case: higher hit rate wins
        if (!found || result.hitRate > best.hitRate) {

            best = result;
            found = true;
        }

        // Tie case: prefer RAMP
        else if (
            result.hitRate == best.hitRate &&
            result.algorithm == "RAMP"
        ) {

            best = result;
        }
    }

    return best;
}


// ============================================================
// Winners
// ============================================================

void printWinners(
    const std::vector<Result>& results,
    const std::vector<Workload>& workloads
) {

    std::cout << '\n';

    loadingDots(
        "  Analyzing workload performance",
        300
    );

    std::cout << '\n';

    std::cout
        << BOLD
        << WHITE
        << "BEST POLICY BY WORKLOAD\n"
        << RESET;

    std::cout
        << GRAY
        << "------------------------------------------------------------\n"
        << RESET;


    for (const auto& workload : workloads) {

        Result best =
            findBest(results, workload.name);

        std::cout
            << "  "
            << CYAN
            << std::setw(12)
            << std::left
            << workload.name
            << RESET
            << " -> ";

        if (best.algorithm == "RAMP") {

            std::cout
                << MAGENTA
                << BOLD
                << best.algorithm
                << RESET;
        }
        else {

            std::cout
                << GREEN
                << best.algorithm
                << RESET;
        }

        std::cout
            << "  ("
            << std::fixed
            << std::setprecision(2)
            << best.hitRate
            << "% hit rate)\n";

        sleepMs(250);
    }

    std::cout
        << GRAY
        << "------------------------------------------------------------\n"
        << RESET;
}


// ============================================================
// Overall summary
// ============================================================

void printOverallSummary(
    const std::vector<Result>& results
) {

    std::cout << '\n';

    loadingDots(
        "  Calculating aggregate performance",
        300
    );

    std::cout << '\n';

    std::cout
        << BOLD
        << WHITE
        << "OVERALL PERFORMANCE\n"
        << RESET;

    std::cout
        << GRAY
        << "------------------------------------------------------------\n"
        << RESET;


    std::vector<std::string> algorithms = {
        "LRU",
        "FIFO",
        "LFU",
        "ARC",
        "RAMP"
    };


    for (const auto& algorithm : algorithms) {

        double totalHitRate = 0.0;

        int count = 0;

        for (const auto& result : results) {

            if (result.algorithm == algorithm) {

                totalHitRate += result.hitRate;
                count++;
            }
        }

        if (count == 0) {
            continue;
        }

        double average =
            totalHitRate / count;


        std::string color;

        if (algorithm == "RAMP") {
            color = MAGENTA;
        }
        else if (algorithm == "ARC") {
            color = CYAN;
        }
        else {
            color = WHITE;
        }


        std::cout
            << "  "
            << color
            << std::left
            << std::setw(8)
            << algorithm
            << RESET
            << " average hit rate: ";


        if (average >= 50.0) {
            std::cout << GREEN;
        }
        else if (average >= 30.0) {
            std::cout << YELLOW;
        }
        else {
            std::cout << RED;
        }


        std::cout
            << std::fixed
            << std::setprecision(2)
            << average
            << "%"
            << RESET
            << '\n';

        sleepMs(200);
    }


    std::cout
        << GRAY
        << "------------------------------------------------------------\n"
        << RESET;
}


// ============================================================
// CSV
// ============================================================

void writeCSV(
    const std::vector<Result>& results,
    const std::string& filename
) {

    std::ofstream file(filename);

    if (!file.is_open()) {

        throw std::runtime_error(
            "Could not create " + filename
        );
    }

    file
        << "Workload,Algorithm,Accesses,Hits,Misses,"
        << "Evictions,HitRate,ExecutionTimeMs\n";


    for (const auto& result : results) {

        file
            << result.workload << ','
            << result.algorithm << ','
            << result.accesses << ','
            << result.hits << ','
            << result.misses << ','
            << result.evictions << ','
            << std::fixed
            << std::setprecision(4)
            << result.hitRate << ','
            << result.executionTimeMs
            << '\n';
    }

    file.close();
}


// ============================================================
// Main
// ============================================================

int main() {

    const int CACHE_SIZE = 3;


    std::vector<Workload> workloads = {

        {"Mixed",      "workloads/mixed.txt"},
        {"Sequential", "workloads/sequential.txt"},
        {"Looping",    "workloads/looping.txt"},
        {"Frequency",  "workloads/frequency.txt"},
        {"Recency",    "workloads/recency.txt"}

    };


    try {

        // ----------------------------------------------------
        // Startup
        // ----------------------------------------------------

        printHeader();

        sleepMs(1000);

        loadingDots(
            "Initializing cache engine",
            500
        );

        sleepMs(500);

        loadingDots(
            "Loading replacement policies",
            500
        );

        sleepMs(500);

        printConfiguration(
            CACHE_SIZE,
            static_cast<int>(workloads.size())
        );

        sleepMs(700);


        // ----------------------------------------------------
        // Simulation
        // ----------------------------------------------------

        std::vector<Result> results;


        std::cout
            << BOLD
            << WHITE
            << "INITIALIZING EXPERIMENT\n"
            << RESET
            << '\n';


        for (const auto& workload : workloads) {

            std::cout
                << CYAN
                << BOLD
                << "  WORKLOAD: "
                << workload.name
                << RESET
                << '\n';


            loadingDots(
                "    Loading trace",
                250
            );


            sleepMs(400);


            std::cout
                << "    Running LRU..."
                << std::flush;

            sleepMs(300);

            results.push_back(
                runAlgorithm<LRU>(
                    workload.name,
                    "LRU",
                    workload.file,
                    CACHE_SIZE
                )
            );

            std::cout
                << GREEN
                << " DONE"
                << RESET
                << '\n';


            std::cout
                << "    Running FIFO..."
                << std::flush;

            sleepMs(300);

            results.push_back(
                runAlgorithm<FIFO>(
                    workload.name,
                    "FIFO",
                    workload.file,
                    CACHE_SIZE
                )
            );

            std::cout
                << GREEN
                << " DONE"
                << RESET
                << '\n';


            std::cout
                << "    Running LFU..."
                << std::flush;

            sleepMs(300);

            results.push_back(
                runAlgorithm<LFU>(
                    workload.name,
                    "LFU",
                    workload.file,
                    CACHE_SIZE
                )
            );

            std::cout
                << GREEN
                << " DONE"
                << RESET
                << '\n';


            std::cout
                << "    Running ARC..."
                << std::flush;

            sleepMs(300);

            results.push_back(
                runAlgorithm<ARC>(
                    workload.name,
                    "ARC",
                    workload.file,
                    CACHE_SIZE
                )
            );

            std::cout
                << GREEN
                << " DONE"
                << RESET
                << '\n';


            std::cout
                << "    Running RAMP..."
                << std::flush;

            sleepMs(300);

            results.push_back(
                runAlgorithm<RAMP>(
                    workload.name,
                    "RAMP",
                    workload.file,
                    CACHE_SIZE
                )
            );

            std::cout
                << MAGENTA
                << " DONE"
                << RESET
                << '\n';


            std::cout << '\n';

            sleepMs(700);
        }


        // ----------------------------------------------------
        // Processing
        // ----------------------------------------------------

        std::cout << '\n';

        progressBar(
            "  Processing experiment",
            30,
            70
        );

        sleepMs(500);


        // ----------------------------------------------------
        // Results
        // ----------------------------------------------------

        printResultsTable(results);

        printWinners(
            results,
            workloads
        );

        printOverallSummary(results);


        // ----------------------------------------------------
        // CSV export
        // ----------------------------------------------------

        std::cout << '\n';

        loadingDots(
            "  Exporting experiment results",
            400
        );

        sleepMs(500);

        writeCSV(
            results,
            "results.csv"
        );


        std::cout
            << '\n'
            << GREEN
            << BOLD
            << "  [OK] Results saved to results.csv"
            << RESET
            << '\n';


        sleepMs(500);


        std::cout
            << '\n'
            << CYAN
            << BOLD
            << "============================================================\n"
            << "                 SIMULATION COMPLETE\n"
            << "============================================================\n"
            << RESET
            << '\n';


        sleepMs(500);

    }


    catch (const std::exception& e) {

        std::cerr
            << '\n'
            << RED
            << BOLD
            << "ERROR: "
            << RESET
            << e.what()
            << '\n';

        return 1;
    }


    return 0;
}
