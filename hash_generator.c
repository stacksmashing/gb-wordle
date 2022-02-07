/*
 * This file is used to generate the bloom filter for word
 * checking.
 * Build: clang -o hash_generator ../bloom.c hash_generator.c
 */

#include <stdio.h>

#include "bloom.h"

int main(int argc, char *argv[]) {
    unsigned int bloom_size = 5000;
	bloom_t bloom = bloom_create(bloom_size);
    FILE *f = fopen(argv[1], "r");
    char wordbuf[6];
    wordbuf[5] = 0;
    int invalid = 0;
    while(1) {

        if(fread(wordbuf, 5, 1, f) != 1) {
            break;
        }
        int before = bloom_test(bloom, wordbuf);
        bloom_add(bloom, wordbuf);
        int after = bloom_test(bloom, wordbuf);

        if(before == after) {
            invalid ++;
        }
        fread(wordbuf, 1, 1, f); // skip newline
    }
    

    // print contents
    printf("const size_t bloom_data_len = %d;\n", bloom_size);
    printf("uint8_t bloom_data[] = {\n\t");
    uint8_t *bits = bloom->bits;
    for(int i = 0; i < bloom_size; i++) {
        printf("0x%02X, ", bits[i]);
        if((i+1) % 8 == 0) {
            printf("\n\t");
        }
    }
    printf("};\n");
    printf("// Failed to handle %d words.\n", invalid);
}
