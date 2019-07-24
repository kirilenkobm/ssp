/*
Copyright: Bogdan Kirilenko
Dresden, Germany, 2019
kirilenkobm@gmail.com
*/
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
// later:
// #include "SSP_lib.h"
#define CHUNK 5

typedef struct
{
    uint64_t number;
    uint64_t quantity;
} Num_q;


// global
bool v = false;
uint64_t arr_size = 0;
uint64_t *f_max;
uint64_t *f_min;
Num_q *num_count;


void _free_all()
// free all allocated stuff
{
    free(f_max);
    free(f_min);
    free(num_count);
}


void verbose(const char * restrict format, ...)
// show verbose message if v is activated
{
    if(!v) {return;}
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    return;
}


uint64_t *accumulate_sum(uint64_t *arr, uint64_t arr_size)
// create accumulated sum array
{
    uint64_t *res = (uint64_t*)malloc(arr_size * sizeof(uint64_t));
    res[0] = arr[0];
    for (uint64_t i = 1; i < arr_size; i++)
    {
        res[i] = res[i - 1] + arr[i];
    }
    return res;
}


Num_q *count_elements(uint64_t *arr, uint64_t arr_size, uint64_t *q)
// count elements, create array of structs
{
    Num_q *res = (Num_q*)malloc(arr_size * sizeof(Num_q));
    // the first element is 0 -> skip it
    // ititiate with the first element
    uint64_t cur_val = arr[1];
    res[*q].number = cur_val;
    res[*q].quantity = 0;
    for (uint32_t i = 1; i < arr_size; i++)
    // go over elements
    // it is granted that they are arranged as:
    // x x x x y y y z z
    {   
        if (arr[i] < cur_val){
            // new elem
            cur_val = arr[i];  // now initiate next value
            *q = *q + 1;  // cannot do with *q++ by sime reason
            res[*q].number = cur_val;
            res[*q].quantity = 1;
        } else {
            // the same value, just increase the quantity
            res[*q].quantity++;
        }
    }
    // shrink array size
    size_t shrinked = sizeof(Num_q) * (*q + CHUNK);
    res = (Num_q*)realloc(res, shrinked);
    // terminate the sequence with 0
    *q = *q + 1;
    res[*q].number = 0;
    res[*q].quantity = 0;
    return res;
}


Num_q *_get_zero_num_q(uint64_t elem_num)
// create empty counter
{
    Num_q *res = (Num_q*)malloc(elem_num * sizeof(Num_q));
    for (uint64_t i = 0; i < elem_num; i++)
    {
        res[i].number = num_count[i].number;
        res[i].quantity = 0;
    }
    return res;
}



void add_to_counter(Num_q *counter, uint64_t *arr, uint64_t arr_size)
// add elements to the counter
{
    uint64_t counter_ind = 0;
    uint64_t arr_ind = 0;
    while (arr_ind < arr_size)
    // both are sorted
    {
        if (arr[arr_ind] == counter[counter_ind].number){
            counter[counter_ind].quantity++;
            arr_ind++;
        } else if (arr[arr_ind] < counter[counter_ind].number){
            counter_ind++;
        } else {
            // must never happen;
            printf("IT HAPPENED!");
            break;
        }
    }
}


uint64_t arr_sum(arr, up_to)
// return array sum
{
    if (up_to == 0){return 0;}
    uint64_t res = 0;
    for (uint64_t i = 0; i < up_to; i++){res += arr[i];}
    return res;
}

uint64_t *find_path(uint64_t sub_size, uint64_t *prev_path, uint64_t prev_p_len,
                    uint64_t _cur_val, uint64_t _cur_ind, uint64_t elem_num)
// the core function -> finds a path
{
    // initiate values
    uint64_t *path = (uint64_t*)malloc(sub_size * sizeof(uint64_t));
    Num_q *path_count = _get_zero_num_q(elem_num);
    uint64_t pos_left;
    uint64_t path_len = prev_p_len;
    uint64_t cur_val = _cur_val;
    uint64_t cur_ind = _cur_ind;

    if (prev_path != NULL){
        // add existing part to answer
        for (uint64_t i = 0; i < prev_p_len; i++){path[i] = prev_path[i];}
        pos_left = sub_size - prev_p_len;
        // add exitsing values to counter
        add_to_counter(path_count, prev_path, prev_p_len);
    } else {pos_left = sub_size;}

    // create local f_max and f_min
    uint64_t *f_max_a = accumulate_sum(f_max, arr_size);
    uint64_t *f_min_a = accumulate_sum(f_min, arr_size);

    for (uint64_t i = 0; i < pos_left; i++)
    // the main loop, trying to add the next element
    {
        bool passed = false;
        uint64_t prev_sum = arr_sum(path, path_len);
        
    }
    free(f_max_a);
    free(f_min_a);
    return path;
}


uint64_t *solve_SSP(uint64_t *in_arr, uint64_t _arr_size, uint64_t sub_size,
                    uint64_t req_sum, bool _v)
// what we should call
{
    // allocate f_max and f_min
    arr_size = _arr_size;
    size_t f_max_min_size = sizeof(uint64_t) * (arr_size + CHUNK);
    // the numbers were actually pre-sorted, so f_min is defined just for explicity
    f_max = (uint64_t*)malloc(f_max_min_size);
    f_min = (uint64_t*)malloc(f_max_min_size);
    f_max[0] = 0;
    f_min[0] = 0;
    for (uint64_t i = 0; i < arr_size; i++){f_min[i + 1] = in_arr[i];}
    for (uint64_t i = 1, j = arr_size; i < arr_size + 1; i++, j--){f_max[j] = f_min[i];}
    arr_size++;  // arrays were started from 0, so size +1
    v = _v;  // maybe there's a better solution to implement verbosity
    // now count the elements
    uint64_t elem_num = 0;
    num_count = count_elements(f_max, arr_size, &elem_num);

    // now find the first path
    uint64_t cur_ind = 0;
    uint64_t cur_val = num_count[cur_ind].number;

    uint64_t *first_path = find_path(sub_size, NULL, 0, cur_val, cur_ind, elem_num);

    // allocate the result and fill it
    uint64_t *answer = (uint64_t*)calloc(sub_size, sizeof(uint64_t));
    _free_all();
    return answer;
}
