/*
 * By Sanhu Li
 *
 * CS 441/541: Page Fault Algorithm Simulator (Project 4)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/******************************
 * Defines
 ******************************/
#define MAX_LINE 1024

/******************************
 * Structures
 ******************************/
typedef struct {
    int val;
    char rw;
} page;

/******************************
 * Global Variables
 ******************************/


/******************************
 * Function declarations
 ******************************/
long alg_optimal(page *pages, long n, int num_frames);
long find_optimal_victim(page *pages, long n, page *frames, int num_frames);

long alg_fifo(page *pages, long n, int num_frames);

long alg_lru(page *pages, long n, int num_frames);
long find_lru_victim(page *frames, int *timestamps, int num_frames, long n);

long alg_lru_sc(page *pages, long n, int num_frames);

long alg_lru_esc(page *pages, long n, int num_frames);
void print_frames(page *frames, int num_frames, bool ignore_ref, bool ignore_write, int *ref_bits, int *write_bits, long idx);
int next(int cur, int max);

long contains(page *frames, long n, int p_val);

page build_page(int val, char rw);

void usage();