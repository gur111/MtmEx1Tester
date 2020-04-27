#include "utils.h"

#include <stdlib.h>
#include <string.h>

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
    char *string =
        "abcdefghijklmnopqrstuvwxyz ";
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