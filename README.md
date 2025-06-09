# SAT Solver Suite

This repository provides a modular and didactic C++ implementation of four classical SAT solving algorithms:

- **Resolution**
- **Davis-Putnam (DP)**
- **Davis-Putnam-Logemann-Loveland (DPLL)**
- **DPLL with Conflict-Driven Clause Learning (DPLL-CDCL)**

A **clause generator** is also included to produce CNF test instances. This suite is designed for educational use and empirical comparison of SAT solver algorithms.

## Overview

SAT (Boolean Satisfiability Problem) is a canonical NP-complete problem central to many domains like hardware verification, artificial intelligence, and cryptography. This project implements classical SAT solvers and a modern CDCL-enhanced solver to showcase algorithmic strategies and their comparative performance.

## Project Structure

- `resolution.cpp`: Classical Resolution algorithm using clause-pair resolution.
- `dp.cpp`: Implements Davis-Putnam algorithm with systematic variable elimination.
- `dpll.cpp`: DPLL with backtracking, unit propagation, and pure literal elimination.
- `dpll_cdcl.cpp`: Enhanced DPLL solver.
- `clausegen.cpp`: Generator for random CNF problems in DIMACS format.

## Build Instructions

Requires a C++11-compatible compiler.

Example build and execution:
```bash
g++ dp.cpp -o dp
./dpll_cdcl example.txt
