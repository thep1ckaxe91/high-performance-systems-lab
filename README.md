# High-Performance Systems Lab ⚡

![Language](https://img.shields.io/badge/Language-C++20-blue?style=flat-square)
![Platform](https://img.shields.io/badge/Platform-Linux%20(Kernel%205.x+)-black?style=flat-square)
![Focus](https://img.shields.io/badge/Focus-Systems%20%26%20Performance-red?style=flat-square)

## Overview
This repository serves as a research laboratory for **Systems Engineering** concepts, moving beyond high-level DevOps abstractions to explore *how* software interacts with hardware and the kernel. 

The goal is to bridge the gap between "Scripting" and "Engineering" by building high-throughput, low-latency mechanisms from scratch.

## Core Research Modules

### [01-hardware-sympathy](./01-hardware-sympathy)
**Focus:** Memory Latency & Concurrency
Exploring the cost of abstractions.
* **False Sharing:** Benchmarking CPU cache line contention and optimization via `alignas` padding.
* **Lock-Free Structures:** Implementing Ring Buffers using C++ Atomics to bypass syscall overhead.

### [02-linux-internals](./02-linux-internals)
**Focus:** Kernel Isolation & Virtualization
Demystifying containers by rebuilding them.
* **Mini-Runtime:** A custom container runtime built using `clone()`, `CLONE_NEWPID`, and `cgroups`.
* **Namespace Inspection:** Deep auditing of Process Control Blocks (PCB) in production environments.

### [03-networking](./03-networking)
**Focus:** Non-Blocking I/O & Scale
Handling 10k+ concurrent connections on a single thread.
* **Event Loops:** Implementing a `sys/epoll` TCP server (Redis-style architecture).
* **Zero-Copy Networking:** Reducing user-kernel space context switches.

### [04-observability](./04-observability)
**Focus:** Kernel Tracing
Debugging systems without stopping them.
* **eBPF Probes:** Writing kernel-space programs to trace syscalls and file I/O in real-time.

## Environment & Toolchain
* **OS:** Arch Linux / EndeavourOS
* **Compiler:** Clang / GCC (C++20)
* **Analysis:** `perf`, `valgrind`, `bcc` (BPF Compiler Collection)

## Usage
Each module contains its own standalone Makefiles/CMakeLists. 
```bash
# Example: Running the False Sharing Benchmark
cd 01-hardware-sympathy
g++ -O3 -pthread false_sharing.cpp -o bench
./bench
