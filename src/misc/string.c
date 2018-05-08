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

void main(int argc, char **argv) {
    char * p;
    char * s = "123456";
    char * a = NULL;
    char * b = NULL;
    printf("s len is :%d\n", strlen(s));
    printf("s addr is :%p\n", s);
    printf("s addr is :%p\n", p);
    if (a == b) {
        printf("NULL can compare");
    }
    printf("EAGAIN is %d\n", strerror(EAGAIN));
}
