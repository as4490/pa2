#!/usr/bin/env python3
import random
from typing import Callable

#
# data functions all calculate matrix entries in row major order. this is
# independent of the way the entries are stored in the buffer.
#
def data_func_all_zeroes(row_idx: int, col_idx: int, R: int, C: int) -> int:
    return 0

def data_func_identity(row_idx: int, col_idx: int, R: int, C: int) -> int:
    return 1 if row_idx == col_idx else 0

def data_func_random(row_idx: int, col_idx: int, R: int, C: int) -> int:
    return random.randint(0, 2**64 - 1)

def data_func_enumerated_rows(row_idx: int, col_idx: int, R: int, C: int) -> int:
    return row_idx * C + col_idx

def data_func_enumerated_cols(row_idx: int, col_idx: int, R: int, C: int) -> int:
    return col_idx * R + row_idx

# creates raw binary byte_arrays that can be used as input for PA2 according to the file specification
# for a nontrivial matrix, entries are enumerated rows/columns
def serialize_matrix(is_row_major : bool, is_all_zeroes : bool, is_identity : bool, R : int, C : int, matrix: list) -> bytearray:
    assert R >= 0 and C >= 0 and R <= 256 and C <= 256
    assert not (is_all_zeroes and is_identity)
    assert not (is_identity and (R != C))

    # fill in the buffer based on the calculated matrix entries
    byte_array = bytearray(b'PA2M')
    props = 0
    if is_row_major:
        props |= 1
    if is_all_zeroes:
        props |= 2
    if is_identity:
        props |= 4

    byte_array.extend(props.to_bytes(2, 'big'))
    byte_array.extend(R.to_bytes(1, 'big'))
    byte_array.extend(C.to_bytes(1, 'big'))

    if not is_identity and not is_all_zeroes:
        if is_row_major:
            for r in range(R):
                for c in range(C):
                    val = matrix[r * C + c]
                    byte_array.extend(val.to_bytes(8, 'big'))
        else:
            for c in range(C):
                for r in range(R):
                    val = matrix[r * C + c]
                    byte_array.extend(val.to_bytes(8, 'big'))

    return byte_array

matrix_definitions = {
    # NxN identity matrices
    "identity_rowmaj_5x5": (True, False, True, 5, 5, data_func_identity),
    "identity_colmaj_5x5": (False, False, True, 5, 5, data_func_identity),

    # NxM zero matrices
    "zeroes_rowmaj_10x5": (True, True, False, 10, 5, data_func_all_zeroes),
    "zeroes_colmaj_5x10": (False, True, False, 5, 10, data_func_all_zeroes),

    # NxM sequentially enumerated matrices (e.g., [0, 1, 2, 3, ..., N])
    "enumerated_rowmaj_5x5": (True, False, False, 5, 5, data_func_enumerated_rows),
    "enumerated_colmaj_5x5": (False, False, False, 5, 5, data_func_enumerated_rows)
}

if __name__ == "__main__":
    for matrix_name, (is_row_major, is_all_zeroes, is_identity, R, C, data_func) in matrix_definitions.items():

        # calculate the real matrix elements (in row-major) that we want to serialize
        matrix = []
        for r in range(R):
            for c in range(C):
                val = data_func(r, c, R, C)
                matrix.append(val)

        # encode the matrix in the PA2 binary buffer format
        byte_array = serialize_matrix(is_row_major, is_all_zeroes, is_identity, R, C, matrix)

        # byte_arrays are output as C-style arrays so that we don't need to bother with file I/O in PA2
        print("uint8_t {}[] = {{{}}};".format(matrix_name, ", ".join([format(b, "#04x") for b in byte_array])))
        print("int64_t {}{}[] = (int64_t[]){{{}}};".format(matrix_name, "_entries", ", ".join(map(str, matrix))))
        print("")

    print("Now copy/paste these into your main.c file to run as test cases!")