/*
 * Programming Assignment 2
 * CS 211 Spring 2026 (Sections 5-8)
 */
#include <stdio.h>
#include <inttypes.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "pa2.h"

/******************************************************************************
 * Provided functions: consult these as examples for how to work with the data
 ******************************************************************************/
void pa2_print_matrix(const char *prefix, struct matrix mat, bool print_hex)
{
    printf("%srows: %"PRIu8", n_cols: %"PRIu8", data address: %p\n", prefix, mat.n_rows, mat.n_cols, mat.data);
    if(mat.data == NULL)
        return;

    for(uint8_t r = 0; r < mat.n_rows; r++)
    {
        printf("%*c", (int)strlen(prefix), ' ');
        for(uint8_t c = 0; c < mat.n_cols; c++)
            printf(print_hex ? "    0x%"PRIx64"" : "    %"PRIi64"", mat.data[r * mat.n_cols + c]);
        printf("\n");
    }
}

/******************************************************************************
 * Your Assignment:
 *     Implement the assigned functions declared in pa2.h. 
 ******************************************************************************/

/*
 * Checks whether computed == expected, accounting for:
 *   (1) matrix dimensions
 *   (2) element values
 *   (3) NULLity of each matrix data pointer
 */
bool pa2_check_matrices_are_equal(struct matrix expected, struct matrix computed)
{
    if ( expected.n_cols != computed.n_cols) {
        return false;
    }

    if ( expected.n_rows != computed.n_rows) {
        return false;
    }

    if (expected.data == NULL && computed.data == NULL) {
        return true;
    }

    if (expected.data == NULL) {
        return false;
    }
    
    if (computed.data == NULL) {
        return false;
    }

    uint64_t Matrice = (uint64_t)expected.n_rows * (uint64_t)expected.n_cols;
    for (uint64_t i = 0; i < Matrice; i = i + 1)
    {
        if (expected.data[i] != computed.data[i])
            return false;
    }
    return true;
}

/*
 * Swaps the endianness of an int64_t byte by byte.
 */
int64_t pa2_swap_endianness_of_int64(int64_t x)
{
    int64_t s = 0;

    for (int i = 0; i < 8; i = i + 1) {
        int64_t b = (x >> (i * 8)) & 0xFF;
        s |= b << ((7 - i) * 8);
    }
    return s;
}

/*
 * Rearranges elements in mat.data so that row-major becomes column-major
 * or vice-versa. Does an in-place transpose of the data array.
 *
 * If data is NULL or dimensions are 0, does nothing.
 * Uses a temporary allocation that is freed before returning.
 */
void pa2_swap_row_major_vs_col_major(struct matrix mat)
{
    if (mat.data == NULL) {
            return;
    } 
    
    uint32_t M = (uint32_t)mat.n_rows * (uint32_t)mat.n_cols;

    if (M == 0) {
        return;
    }

    int64_t *t = malloc(M * sizeof(int64_t));

    if (t == NULL) {
        return;
    }
    
    for (uint8_t i = 0; i < mat.n_rows; i = i + 1)
        for (uint8_t j = 0; j < mat.n_cols; j = j + 1) {
            uint32_t x = (uint32_t)i * mat.n_cols + j;
            uint32_t y = (uint32_t)j * mat.n_rows + i;
            t[y] = mat.data[x];
        }
    memcpy(mat.data, t, M * sizeof(int64_t));
    free(t);
}

/*
 * Allocates a zero matrix of size n_rows x n_cols.
 * Returns empty matrix {0,0,NULL} on invalid dimensions (0 rows or 0 cols).
 */
struct matrix pa2_create_zero_matrix(uint8_t n_rows, uint8_t n_cols)
{
    struct matrix zero = { 0, 0, NULL };

    if (n_rows == 0) {
        return zero;
    }

    if (n_cols == 0) {
        return zero;
    }

    uint32_t new = (uint32_t)n_rows * (uint32_t)n_cols;
    zero.data = calloc(new, sizeof(int64_t));
    if (zero.data == NULL) {
        return zero;
    }

    zero.n_rows = n_rows;
    zero.n_cols = n_cols;
    return zero;


}

/*
 * Parses a struct matrix from a raw byte array in PA2 serialized format.
 * Always returns the matrix in ROW-MAJOR order.
 * Returns empty matrix on any error/malformed input.
 *
 * Binary format:
 *   bytes [0..3]  : magic = {'P','A','2','M'}
 *   bytes [4..5]  : uint16_t props (big-endian)
 *                     bit[0]: 1=row-major, 0=col-major
 *                     bit[1]: 1=all zeros
 *                     bit[2]: 1=identity matrix
 *                     bits[15:3]: reserved (must be 0)
 *   byte  [6]     : uint8_t R (num_rows)
 *   byte  [7]     : uint8_t C (num_cols)
 *   bytes [8...]  : int64_t data[] in big-endian
 *                   (absent if all-zeros or identity)
 */
struct matrix pa2_deserialize_matrix_in_row_major_order(uint8_t *byte_array, uint32_t byte_array_length)
{
    struct matrix badone = { 0, 0, NULL };

    if (byte_array == NULL) {
        return badone;
    }
    
    if (byte_array_length < 8) {
        return badone;
    }
    
    if (byte_array[0] != 'P') {
        return badone;
    }

    if (byte_array[1] != 'A') {
        return badone;
    }

    if (byte_array[2] != '2') {
        return badone;
    }

    if (byte_array[3] != 'M') {
        return badone;
    }

    uint16_t h = byte_array[4];
    uint16_t l = byte_array[5];
    uint16_t p = (h << 8) | l;

    if ((p >> 3) != 0) {
        return badone;
    }

    // extract bits
    bool RM = (p >> 0) & 1;
    bool AZ = (p >> 1) & 1;
    bool I = (p >> 2) & 1;

    if (AZ && I) {
        return badone;
    }

    // parse dimensions
    uint8_t r = byte_array[6];
    uint8_t c = byte_array[7];

    if (r ==0) {
        return badone;
    }
    
    if (c == 0) {
        return badone;
    }

    if (I && r != c) {
        return badone;
    }

    uint32_t t = (uint32_t)r * (uint32_t)c;
    uint32_t e;
    if (AZ || I) {
        e = 8;
    } 
    
    else {
        e = 8 + t * 8;
    }

    if (byte_array_length < e) {
            return badone;
    }

    int64_t *data = calloc(t, sizeof(int64_t));
    if (data == NULL) {
        return badone;
    }

    if (I == true) {
        for (uint8_t i = 0; i < r; i = i + 1) {
            data[(uint32_t)i * c + i] = 1;
        }
    } else if (AZ == false) {
        for (uint32_t i = 0; i < t; i = i + 1) {
            uint32_t o = 8 + i * 8;
            int64_t x;

            memcpy(&x, byte_array + o, sizeof(int64_t));
            x = pa2_swap_endianness_of_int64(x);
            if (RM == true) {
                data[i] = x;
            } else {
                uint32_t row = i % r;
                uint32_t col = i / r;
                uint32_t j = row * c + col;
                data[j] = x;
            }
        }
        
    }
    return (struct matrix){ r, c, data };
}

/*
 * Computes C = A * B (matrix multiply).
 * A is (m x n), B is (n x p), result is (m x p).
 * Returns empty matrix if dimensions don't match or inputs are invalid.
 */
struct matrix pa2_matrix_multiply(struct matrix matrix_A, struct matrix matrix_B)
{
    struct matrix empty = { 0, 0, NULL };

    // validate inputs
    if (matrix_A.data == NULL) {
        return empty;
    }

    if (matrix_B.data == NULL) {
        return empty;
    }

    
    if (matrix_A.n_cols != matrix_B.n_rows) {
        return empty;
    }
    uint8_t x = matrix_A.n_rows;
    uint8_t y = matrix_A.n_cols;
    uint8_t z = matrix_B.n_cols;

    int64_t *data = calloc((uint32_t)x * (uint32_t)z, sizeof(int64_t));
    if (data == NULL) {
            return empty;
    }

    // standard O(m*n*p) matrix multiply
    for (uint8_t i = 0; i < x; i = i + 1) {
        for (uint8_t j = 0; j < z; j++) {
            int64_t sum = 0;
            for (uint8_t k = 0; k < y; k++)
            {
                int64_t a = matrix_A.data[(uint32_t)i * y + k];
                int64_t b = matrix_B.data[(uint32_t)k * z + j];
                int64_t product = a * b;
                sum = sum + product;
            }
            data[(uint32_t)i * z + j] = sum;
        }
    }

    return (struct matrix){ x, z, data };
}

/*
 * Frees the data pointer of a matrix if non-NULL.
 */
void pa2_release_matrix(struct matrix mat)
{
    if (mat.data != NULL)
        free(mat.data);
}