/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/28/2017 11:04:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

int main(int argc, char **argv) {
    int sum = 0;
    int i = 0;
    for (i=0; i<2018; i++) {
        if (i%2!=0 && i%3!=0 && i%5!=0) {
            sum += i;
        }
    }
    printf("sum is %d\n", sum);
    return 0;
}
