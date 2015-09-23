#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/mman.h>

#define ANIMATE 1

static long in_size = 0;

char *read(char* name) {
    FILE *in = fopen(name, "rb");
    if (in!=NULL) {
        fseek(in, 0L, SEEK_END);
        in_size = ftell(in);
        rewind(in);
        char *buff = (char*) mmap(NULL, in_size, PROT_READ, MAP_PRIVATE, fileno(in), 0);
        fclose (in);
        return buff;
    } else {
        fputs("Error opening file", stderr);
        return NULL;
    }
}

void scan(char *buff, int *width, int *height) {
    char c;
    *width = 0;
    *height = 1;
    int i = 0;

    while((c = *(buff++))) {
        if(c == '\n') {
            (*height)++;
            if(i > *width) {
                *width = i;
            }
            i = 0;
        } else {
            i++;
        }
    }
    if(i > *width) {
        *width = i;
    }
}

void copy(char* buff, char* dest, int line_width) {
    int i = 0;
    while(*buff && (*(dest) = *(buff++))) {
        if(*dest == '\n') {
            *dest = ' ';
            dest += line_width - i + 1;
            i = 0;
        } else {
            i++;
            dest++;
        }
    }
}

void insert_0(char *dest, int width, int height) {
    while((*(dest + (height) * (width + 1) - 1) = '\0') || height--) ;
}

void print(char *buff, int width, int height) {
    char *it = buff;
    while(it < buff + height * width + height) {
        puts(it);
        it += width + 1;
    }
}

char *get(char *buff, int row, int col, int width) {
    return buff + row * (width + 1) + col;
}

void get_neighbours(char *neighbours, char *current, int row, int col, int width, int height) {
    char  c;
    if(row > 0) {
        if(col > 0 && (c = *get(current, row - 1, col - 1, width)) != ' ') {
            *(neighbours++) = c;
        }
        if((c = *get(current, row - 1, col, width)) != ' ') {
            *(neighbours++) = c;
        }
        if(col < width - 1 && (c = *get(current, row - 1, col + 1, width)) != ' ') {
            *(neighbours++) = c;
        }
    }
    if(col > 0 && (c = *get(current, row, col - 1, width)) != ' ') {
        *(neighbours++) = c;
    }
    if(col < width - 1 && (c = *get(current, row, col + 1, width)) != ' ') {
        *(neighbours++) = c;
    }
    if(row < height - 1) {
        if(col > 0 && (c = *get(current, row + 1, col - 1, width)) != ' ') {
            *(neighbours++) = c;
        }
        if((c = *get(current, row + 1, col, width)) != ' ') {
            *(neighbours++) = c;
        }
        if(col < width - 1 && (c = *get(current, row + 1, col + 1, width)) != ' ') {
            *(neighbours++) = c;
        }
    }
}
char get_next(char *current, int row, int col, int width, int height) {
    char *neighbours = calloc(9, sizeof(char));
    get_neighbours(neighbours, current, row, col, width, height);
//    printf("(%d, %d): ", row, col);
//    puts(neighbours);

    int n = strlen(neighbours);

    char cell = *get(current, row , col, width);
    char newborn = neighbours[rand() % 3];
    free(neighbours);

    if(n < 2 || n > 3) {
        return ' ';
    } else if(n == 3 && cell == ' ') {
        return newborn;
    } else {
        return cell;
    }
}

void live(char *current, char *next, int width, int height) {
    for(int row = 0; row < height; row++) {
        for(int col = 0; col < width; col++) {
            *get(next, row, col, width) = get_next(current, row, col, width, height);
        }
    }
}

int main(int argc, char **argv) {
    char *current;
    char *next;
    srand(time(NULL));

    char *buff = read(argv[1]);

    int width, height;
    scan(buff, &width, &height);

    int  size = height * width + height;
    current = malloc(size);
    next = malloc(size);
    memset(current, ' ', size);

    copy(buff, current, width);
    munmap(buff, in_size);
    insert_0(current, width, height);
    insert_0(next, width, height);

//    print(current, width, height);
    live(current, next, width, height);
    print(next, width, height);
#if ANIMATE
    while(true) {
        buff = next;
        next = current;
        current = buff;

        nanosleep((const struct timespec[]){{0, 1000L*1000L*1000L/10}}, NULL);
        live(current, next, width, height);
        printf("\033[2J\033[1;1H");
        print(next, width, height);
    }
#endif
}