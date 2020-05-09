#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define MALLOC_FAIL_CHANCE 10

char *randString(int length) {
    char *string =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";
    size_t string_len = strlen(string);
    char *random_string = NULL;

    if (length < 1) {
        length = 1;
    }
    random_string = malloc(sizeof(char) * (length + 1));
    if (random_string == NULL) {
        return NULL;
    }
    short key = 0;
    for (int n = 0; n < length; n++) {
        key = rand() % string_len;
        random_string[n] = string[key];
    }

    random_string[length] = '\0';

    return random_string;
}

char *randLowerString(int length) {
    char *string = "abcdefghijklmnopqrstuvwxyz ";
    size_t string_len = strlen(string);
    char *random_string = NULL;

    if (length < 1) {
        length = 1;
    }
    random_string = malloc(sizeof(char) * (length + 1));
    if (random_string == NULL) {
        return NULL;
    }
    short key = 0;
    for (int n = 0; n < length; n++) {
        key = rand() % string_len;
        random_string[n] = string[key];
    }

    random_string[length] = '\0';

    return random_string;
}

void *xmalloc(long size) {
    // The function will fail on the `fail_after`th malloc.
    // Zero mean never simulate failure
    static int fail_after = 0;
    if (size <= 0) {
        fail_after = -size;
        return NULL;
    }

    if (fail_after == 1) {
        fprintf(stderr, "Out of memory simulated\n");
        fail_after = 0;
        return NULL;
    }
    if (fail_after > 0) {
        fail_after--;
    }
    assert(fail_after >= 0);
    return (malloc(size));
}

char *tester_intToStr(int num) {
    char *res = malloc(30);
    assert(res != NULL);
    sprintf(res, "%d", num);
    return res;
}