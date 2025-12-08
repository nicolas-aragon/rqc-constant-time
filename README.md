# RQC-Multi-MS-AG benchmarks

## Overview

This repository provides supporting material to reproduce the results of the paper "Constant-time decoding of Gabidulin codes and their generalizations with application to RQC".  
All timings are expressed in **CPU cycles**. After running `make all`, the compiled binaries are available in the `bin` directory.

The codebase allows you to:

- run a single KEM test,
- benchmark the average execution time of each algorithmic component,
- verify the constant-time behavior of the decoder with respect to error weight,
- inspect how uninitialized data can reveal condition-dependent branching,
- analyze the distribution of computational costs across decoding stages.

Results from the paper were computed with TurboBoost turned off.

---

## Instructions

### Dependencies

This project depends on OpenSSL and Valgrind (for memcheck analyses), and you must update the corresponding library paths directly in the Makefile before building.

### Build

The Makefile provides the following targets:

- **main**  
  Runs the KEM once.

- **main_speed**  
  Executes 10,000 tests and reports the average cycle count for each KEM operation: Key Generation (KG), Encapsulation (Enc), and Decapsulation (Dec).

- **main_const_time**  
  Evaluates the decoder across all error weights strictly below the correction capacity.  
  This tool confirms that decapsulation time shows no correlation with error weight, supporting constant-time behavior.

- **main_poison**  
  Performs decoding with tainted variables in order to check for non constant-time behoviors using Valgrind.

- **main_distribution**  
  Measures and displays the distribution of computational costs across the various steps of the decoding algorithm.

- **main_ldiv**  
  Performs constant-time testing for the Euclidean division routine, in the same spirit as main_const_time, verifying that execution time does not correlate with input weight or structure.

- **all**
  Produces all executables.

- **clean**
  Removes object and executable files.