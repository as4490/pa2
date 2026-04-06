/*
 * Programming Assignment 2
 * CS 211 Spring 2025 (Sections 5-8)
 */
#ifndef PA2_H
#define PA2_H

#include <stdint.h>
#include <stdbool.h>

/**
 * Struct to represent a matrix (dimensions + data)
 * 
 * Note that passing the struct by value copies the elements, including the pointer object
 */
struct matrix
{
    uint8_t n_rows; // number of matrix n_rows  
    uint8_t n_cols; // number of matrix columns  
    int64_t *data; // array of matrix elements in row-major order
};

/******************************************************************************
 * Provided Functions: 
 *     Consult these as examples for how to work with the data
 *     See pa2.c for function descriptions and examples
 ******************************************************************************/
void pa2_print_matrix(const char *prefix, struct matrix mat, bool print_hex);

/******************************************************************************
 * Your Assignment:
 *     Implement the following functions in pa2.c
 *     See pa2.c for function descriptions and examples
 ******************************************************************************/
bool pa2_check_matrices_are_equal(struct matrix expected, struct matrix computed);
int64_t pa2_swap_endianness_of_int64(int64_t x);
void pa2_swap_row_major_vs_col_major(struct matrix mat);
struct matrix pa2_create_zero_matrix(uint8_t n_rows, uint8_t n_cols);
struct matrix pa2_deserialize_matrix_in_row_major_order(uint8_t *byte_array, uint32_t byte_array_length);
struct matrix pa2_matrix_multiply(struct matrix matrix_0, struct matrix matrix_1);
void pa2_release_matrix(struct matrix mat);

#endif /* PA2_H */