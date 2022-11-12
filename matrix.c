#include "matrix.h"
#include <stddef.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <errno.h> /* for ENOSYS */

int matrix_allocate(matrix_t *m, int rows, int columns) {
    /* implement the function here ... */
    if (rows <=0 || columns <=0)
        return -1;

    m->rows = rows;
    m->columns = columns;
    m->content = (int **)malloc(sizeof(int *) * rows);
    if (m->content == NULL)
        return -1;

    for (int i=0; i<rows; i++){
        m->content[i] = (int *)malloc(sizeof(int) * columns);
        if (m->content[i] == NULL){
            matrix_free(m);
            return -1;
        }
    }

    return 0;
}


void matrix_free(matrix_t *m) {
    /* implement the function here ... */
    if (m->content != NULL){
        for (int i = 0; i < m->rows; i++){
            if (m->content[i] != NULL){
                free(m->content[i]);
                m->content[i] = NULL;
            }
        }

        free(m->content);
        m->content = NULL;
    }
}


void matrix_init_zeros(matrix_t *m) {
    /* implement the function here ... */
     if (m->content != NULL){
         for (int i = 0; i < m->rows; i++){
            for (int j = 0; j < m->columns; j++){
                m->content[i][j] = 0;
            }
         }
     }
}


int matrix_init_identity(matrix_t *m){
    /* implement the function here ... */
    if (m->content == NULL || m->rows != m->columns || m->rows <= 0)
        return -1;

     for (int i = 0; i < m->rows; i++){
        for (int j = 0; j < m->columns; j++){
                if (i == j)
                    m->content[i][j] = 1;
                else
                    m->content[i][j] = 0;
        }
     }

     return 0;
}


void matrix_init_rand(matrix_t *m) {
    /* implement the function here ... */
    if (m->content != NULL){
        for (int i = 0; i < m->rows; i++){
            for (int j = 0; j < m->columns; j++)
                m->content[i][j] = rand();
        }
     }
}


int matrix_equal(matrix_t *m1, matrix_t *m2) {
    /* implement the function here ... */
    if (m1->content == NULL || m2->content == NULL)
        return 0;

    if (m1->rows != m2->rows || m1->columns != m2->columns)
        return 0;

    for (int i = 0;  i < m1->rows; i++){
        for (int j = 0; j < m1->columns; j++)
            if (m1->content[i][j] != m2->content[i][j])
                return 0;
    }

    return 1;
}


int matrix_sum(matrix_t *m1, matrix_t *m2, matrix_t *result) {
    /* implement the function here ... */
    if (m1->content == NULL || m2->content == NULL)
        return -1;

    if (m1->rows != m2->rows || m1->columns != m2->columns)
        return -1;

    if (matrix_allocate(result, m1->rows, m1->columns) != 0)
        return -1;

     for (int i=0;  i<m1->rows; i++){
        for (int j=0; j<m1->columns; j++)
            result->content[i][j] = m1->content[i][j] + m2->content[i][j];
    }

    return 0;
}

int matrix_scalar_product(matrix_t *m, int scalar, matrix_t *result) {
    /* implement the function here ... */

    if (m->content == NULL)
        return -1;

    if (matrix_allocate(result, m->rows, m->columns) != 0)
        return -1;

     for (int i=0;  i<m->rows; i++){
        for (int j=0; j<m->columns; j++)
            result->content[i][j] = m->content[i][j] * scalar;
    }

    return 0;
}


int matrix_transposition(matrix_t *m, matrix_t *result) {
    /* implement the function here ... */
    if (m->content == NULL)
        return -1;

    if (matrix_allocate(result,  m->columns, m->rows) != 0)
        return -1;

     for (int i=0;  i<m->rows; i++){
        for (int j=0; j<m->columns; j++)
            result->content[j][i] = m->content[i][j];
    }

    return 0;
}


int matrix_product(matrix_t *m1, matrix_t *m2, matrix_t *result) {
    /* implement the function here ... */
    if (m1->content == NULL || m2->content == NULL)
        return -1;

    if (m1->columns != m2->rows)
        return -1;

    if (matrix_allocate(result, m1->rows, m2->columns) != 0)
        return -1;

     for (int i=0;  i<m1->rows; i++){
        for (int j=0; j<m2->columns; j++){
            result->content[i][j] = 0;
            for (int k = 0; k < m1->columns; k++)
                result->content[i][j] += m1->content[i][k] * m2->content[k][j];
        }
    }

    return 0;

}

int matrix_dump_file(matrix_t *m, char *output_file) {
    /* implement the function here ... */
    FILE *pFile = fopen(output_file, "w");
    if (pFile == NULL)
        return -1;

    if (m->content != NULL){
        for (int i = 0; i < m->rows; i++){
            for (int j = 0; j < m->columns; j++){
                fprintf(pFile, "%d", m->content[i][j]);

                if (j < m->columns - 1)
                    fprintf(pFile, " ");
                else
                    fprintf(pFile, "\n");
            }
        }
    }
    fclose(pFile);

    return 0;
}


int extract_integers_in_str(char *str, int *result){
    char seps[] = " ,";
    char * token = strtok(str, seps);

    int i = 0;
    while (token != NULL){
        if (result != NULL)
            result[i] = atoi(token);

        token = strtok(NULL, seps);
        i++;
    }

    return i;
}


int matrix_allocate_and_init_file(matrix_t *m, char *input_file) {
    /* implement the function here ... */
    FILE *pFile = fopen(input_file, "r");
    if (pFile == NULL)
        return -1;

    char buffer[4096];
    int rows = 0;
    int columns = 0;

    while (!feof(pFile)){
        char *str = fgets(buffer, sizeof(buffer) -1, pFile);
        int numbers = extract_integers_in_str(str, NULL);
        if (numbers == 0)
            continue;

        if (columns == 0)
            columns = numbers;
        else if (numbers != columns){
            fclose(pFile);
            return -1;
        }

        rows++;
    }

    if (matrix_allocate(m, rows, columns) != 0){
        fclose(pFile);
        return -1;
    }

    fseek(pFile, 0, SEEK_SET);
    int i = 0;
    while (!feof(pFile)){
        char *str = fgets(buffer, sizeof(buffer) - 1, pFile);
        int numbers = extract_integers_in_str(str, m->content[i]);
        if (numbers == 0)
            continue;
        i++;
    }

    fclose(pFile);
    return 0;
}
