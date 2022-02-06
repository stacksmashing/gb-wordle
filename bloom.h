#ifndef _BLOOM_H
#define _BLOOM_H
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint32_t (*hash_function)(const char *data);


struct bloom_filter {
    struct bloom_hash *func;
    uint8_t *bits;
    size_t size;
};
typedef struct bloom_filter * bloom_t;
uint32_t djb2(const char *_str);
uint32_t jenkins(const char *_str);
/* Creates a new bloom filter with no hash functions and size * 8 bits. */
bloom_t bloom_create(size_t size);
bloom_t bloom_create_existing(size_t size, uint8_t *bits);
/* Frees a bloom filter. */
void bloom_free(bloom_t filter);
/* Adds a hashing function to the bloom filter. You should add all of the
 * functions you intend to use before you add any items. */
void bloom_add_hash(bloom_t filter, hash_function func);
/* Adds an item to the bloom filter. */
void bloom_add(bloom_t filter, const char *item);
/* Tests if an item is in the bloom filter.
 *
 * Returns false if the item has definitely not been added before. Returns true
 * if the item was probably added before. */
bool bloom_test(bloom_t filter, const char *item);

#endif
