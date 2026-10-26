# RAMP_os — Cache Replacement Algorithm Simulator

## Overview

RAMP_os is a C++-based simulator for studying and evaluating cache replacement algorithms. The project implements multiple replacement strategies and provides workload files for experimenting with different access patterns.

The simulator is designed to help analyze how cache replacement policies behave under different workloads and to support comparative evaluation.

## Implemented Algorithms

- **FIFO (First-In, First-Out):** Replaces the cache entry that has been present the longest.
- **LRU (Least Recently Used):** Replaces the entry that has not been accessed for the longest time.
- **LFU (Least Frequently Used):** Replaces the entry with the lowest access frequency.
- **ARC (Adaptive Replacement Cache):** Adaptively balances recency and frequency to respond to changing access patterns.

## Repository Structure

```text
RAMP_os/
├── include/
│   ├── arc.h
│   ├── cache_line.h
│   ├── fifo.h
│   ├── lfu.h
│   ├── lru.h
│   └── ramp.h
├── src/
│   ├── arc.cpp
│   ├── fifo.cpp
│   ├── lfu.cpp
│   ├── lru.cpp
│   ├── main.cpp
│   └── ramp.cpp
├── workloads/
│   ├── frequency.txt
│   ├── looping.txt
│   ├── mixed.txt
│   ├── recency.txt
│   └── sequential.txt
├── input.txt
├── results.csv
└── README.md
```

## Requirements

- A C++ compiler supporting the C++ standard used by the source code
- GCC/G++ or an equivalent compiler
- Windows PowerShell, Linux terminal, or another compatible development environment

## Build Instructions

### Windows (PowerShell)

Install a C++ compiler such as MinGW-w64 and ensure that `g++` is available in your PATH.

From the project root, run:

```powershell
g++ -std=c++17 -Iinclude src/*.cpp -o ramp_sim.exe
```

Execute the simulator:

```powershell
.\ramp_sim.exe
```

### Linux

Compile the source files:

```bash
g++ -std=c++17 -Iinclude src/*.cpp -o ramp_sim
```

Run the executable:

```bash
./ramp_sim
```

If compilation fails, check the compiler version and the project's actual source dependencies.

## Workloads

The `workloads/` directory contains input traces intended to represent different access patterns:

- `frequency.txt` — frequency-oriented access patterns
- `looping.txt` — repeated accesses over a looping sequence
- `mixed.txt` — mixed access patterns
- `recency.txt` — recency-oriented access patterns
- `sequential.txt` — sequential access patterns

These workloads can be used to investigate how different replacement policies respond to changes in access behavior.

## Input and Output

- `input.txt` — Input data used by the simulator.
- `results.csv` — CSV file containing results from an experiment or simulation run.

The exact input format, execution parameters, and reported metrics depend on the implementation in the source code. Refer to the program's input handling and output generation for the supported configuration.

## Experimental Evaluation

The simulator can be used to compare FIFO, LRU, LFU, and ARC under different access traces and cache configurations.

For a meaningful comparison, keep the workload and cache capacity consistent across algorithms. Record the metrics produced by the implementation, such as cache hits, cache misses, and hit ratio, where supported.

## Research and Academic Use

This project provides an implementation and experimental environment for exploring cache replacement strategies. The repository can be cited in academic work that discusses or evaluates the implementation.

**Source code:** https://github.com/anshgupta8625/RAMP_os

## Future Improvements

- Automated execution across all workloads
- Standardized CSV reporting and result visualization
- Expanded benchmark workloads
- Reproducible experiments across multiple cache capacities
- Additional cache replacement policies

## License

A license has not yet been specified. Please add an appropriate license before permitting reuse or redistribution.

