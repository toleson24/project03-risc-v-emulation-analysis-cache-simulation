[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/xqpHNoA2)
# Project03: RISC-V Emulation - Analysis - Cache Simulation

## About

This repository is a continuation of Lab05, in which a subset of RISC-V instructions were emulated. Here, more instructions are emulated. Below is a complete list of the specification:

> 1. Write an emulator in C for a subset of the RISC-V Instruction Set Architecture (ISA).
> 2. Emulate the instructions required for the following programs:
> 	- `quadratic_s`
>	- `midpoint_s`
>	- `max3_s`
> 	- `to_upper`
> 	- `get_bitseq_s`
> 	- `get_bitseq_signed_s`
> 	- `swap_s`
> 	- `sort_s`
> 	- `fib_rec_s`  
> 3. Continue the logic (decoding and emulating instructions) and state (`struct rv_state_st`) from Lab05.
> 4. Support Dynamic Analysis of instruction execution with the following metrics:
> 	- number of instructions executed (`i_count`)
> 	- number of I-type and R-type instructions executed (`ir_count`)
> 	- number of LOAD instructions executed (`ld_count`)
> 	- number of STORE instructions executed (`st_count`)
> 	- number of jump instructions executed including `j`, `jal`, `jalr` (`j_count`)
> 	- number of conditional branches taken (`b_taken`)
> 	- number of conditional branches not taken (`b_not_taken`)
> 5. Implement a processor cache simulator for the following cache types:
> 	- direct mapped cache with a block size on 1 word (given)
> 	- direct mapped cache with a block size of 4 words
> 	- 4-way set associative cache with a block size of 1 word and LRU slot replacement
> 	- 4-way set associative cache with a block size of 4 words and LRU slot replacement

## Links

Tests: https://github.com/USF-CS631-S24/tests

Autograder: https://github.com/phpeterson-usf/autograder

## Usage

### Compilation and Assembly

For general purpose usage, please use the Makefile.

The Makefile will link and compile the primary objects required for running `project03.c` as the main fucntion. For example,

> ```bash gcc -g -c -o project03.o project03.c```  
> `gcc -g -c -o rv_emu.o rv_emu.c`  
> `gcc -g -c -o cache.o cache.c`  
> `gcc -g -c -o verbose.o verbose.c`  
> `gcc -g -c -o bits.o bits.c`  

To compile the C and Assembly test programs, run

> `as -g -o quadratic_s.o quadratic_s.s`

for the Assembly test program, and run

> `gcc -g -c -o quadratic_c.o quadratic_c.c`

for the C test program.

To create the main test program, run

> `gcc -g -o project03.o rv_emu.o cache.o verbose.o bits.o quadratic_c.o quadratic_s.o`
>  
>  Note: include all `.o` files that will be tested in this command.

### Running

#### Testing the RISC-V emulator:

To test the emulator, run 

> `./program03 quadratic 2 4 6 8`

#### Testing the Dynamic Analysis:

To print the Dynamic Analysis for a particular test program, use the `-a` flag

> `./program03 -a quadratic 2 4 6 8` 

#### Testing the processor caching simualtor:

To test and print the metrics for processor cache simulation, us the `-dm`, and `-sa` flags, for direct mapping and set associative caching respectively.

> The `-dm` flag takes two arguments: slots, and block size  
> `./program03 -dm 32 1 quadratic 2 4 6 8`
>
> The `-sa` flag takes three arguments: slots, block size, and ways  
> `./program03 -sa 32 1 4 quadratic 2 4 6 8`
>  
> For verbose cache printing, use the `-v` flag.

