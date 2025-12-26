# CPU Pipeline + Cache Simulator in C++ + Verilog (in progress)

A cycle-accurate CPU simulator written in C++ that models a 5-stage pipelined processor integrated with a set-associative cache. The simulator focuses on realistic pipeline control, hazard handling, and memory latency effects.

---

## Overview

This project implements a simplified RISC-style CPU with a classic 5-stage pipeline:

IF → ID → EX → MEM → WB

The pipeline is connected to a configurable set-associative cache that introduces realistic memory access latency. Together, they allow exploration of pipeline hazards, stalls, forwarding, and cache behavior.

---

## Features

### CPU Pipeline
- 5-stage pipeline (IF, ID, EX, MEM, WB)
- Load-use hazard detection with stall insertion
- Data forwarding (EX→EX, WB→EX)
- Branch handling with pipeline flush
- Instruction retirement tracking
- CPI calculation
- Register x0 hardwired to zero

### Cache
- n-way set-associative cache (configurable sets and associativity)
- LRU (Least Recently Used) eviction policy
- Word-addressed memory model
- Write-through store policy
- Simulated hit/miss latency
- Cache hit/miss statistics

---

## Instruction Set

- ADD, SUB, ADDI
- LOAD, STORE
- BEQ, JAL
- NOP, HALT

---

## Build and Run

```bash
cd src
g++ main.cpp -o sim
./sim
