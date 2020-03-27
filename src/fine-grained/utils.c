#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

#define BUFSIZE 64

char *
read_line()
{
    char *line;
    line = NULL;

    size_t bufsize = 0;
    getline(&line, &bufsize, stdin);

    return line;
}

char **
split_line(char *line)
{
        char *sep = " \t\r\n\a";
        char **tokens;
        char *token;

        int token_count = 0;

        tokens = malloc(sizeof (char*) * BUFSIZE);
        token = strtok(line, sep);

        while (token != NULL) {
                tokens[token_count] = token;
                token = strtok(NULL, sep);
                token_count++;
        }

        /* Putting a null byte here so we can count the argv eventually */
        tokens[token_count] = NULL;

        return tokens;
}

