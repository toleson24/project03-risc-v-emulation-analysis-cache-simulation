[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/xqpHNoA2)
# Project03: RISC-V Emulation - Analysis - Cache Simulation

## About

This repository is a continuation of Lab05, in which a subset of RISC-V instructions were emulated. Here, more instructions are emulated. Below is a complete list of the specification:

> 1. Write an emulator in C for a subset of the RISC-V Instruction Set Architecture (ISA).
> 2. Emulate the instructions required for the following programs:
> 	- quadratic_s
>	- midpoint_s
>	- max3_s
> 	- to_upper
> 	- get_bitseq_s
> 	- get_bitseq_signed_s
> 	- swap_s
> 	- sort_s
> 	- fib_rec_s 
> 3. Continue the logic (decoding and emulating instructions) and state (struct rv_state_st) from Lab05.
> 4. Support Dynamic Analysis of instruction execution with the following metrics:
> 	a. # of instructions executed (i_count)
> 	b. # of I-type and R-type instructions executed (ir_count)
> 	c. # of LOAD instructions executed (ld_count)
> 	d. # of STORE instructions executed (st_count)
> 	e. # of jump instructions executed including j, jal, jalr (j_count)
> 	f. # of conditional branches taken (b_taken)
> 	g. # of conditional branches not taken (b_not_taken)
> 5. Implement a processor cache simulator for the following cache types:
> 	a. A direct mapped cache with a block size on 1 word (given)
> 	b. A direct mapped cache with a block size of 4 words
> 	c. A 4-way set associative cache with a block size of 1 word and LRU slot replacement
> 	d. A 4-way set associative cache with a block size of 4 words and LRU slot replacement

## Links

Tests: https://github.com/USF-CS631-S24/tests

Autograder: https://github.com/phpeterson-usf/autograder

## Usage


