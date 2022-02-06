#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "bloom.h"

struct bloom_hash {
    hash_function func;
    struct bloom_hash *next;
};


uint32_t djb2(const char *_str) {
	const char *str = _str;
	uint32_t hash = 5381;
	uint8_t c;
	while ((c = *str++)) {
		hash = ((hash << 5) + hash) + (uint32_t)c;
	}
	return hash;
}

// uint32_t jenkins(const void *_str) {
// 	const char *key = _str;
// 	uint32_t hash;
// 	uint32_t i;
// 	while (*key) {
// 		hash += *key;
// 		hash += (hash << 10);
// 		hash ^= (hash >> 6);
// 		key++;
// 	}
// 	hash += (hash << 3);
// 	hash ^= (hash >> 11);
// 	hash += (hash << 15);
// 	return hash;
// }


bloom_t bloom_create_existing(size_t size, uint8_t *bits) {
	bloom_t res = calloc(1, sizeof(struct bloom_filter));
	res->size = size;
	res->bits = bits;
	return res;
}

bloom_t bloom_create(size_t size) {
	bloom_t res = calloc(1, sizeof(struct bloom_filter));
	res->size = size;
	res->bits = malloc(size);
	memset(res->bits, 0, size);
	return res;
}

void bloom_free(bloom_t filter) {
	if (filter) {
		while (filter->func) {
			struct bloom_hash *h = filter->func;
			filter->func = h->next;
			free(h);
		}
		free(filter->bits);
		free(filter);
	}
}

void bloom_add_hash(bloom_t filter, hash_function func) {
	struct bloom_hash *h = calloc(1, sizeof(struct bloom_hash));
	h->func = func;
	struct bloom_hash *last = filter->func;
	while (last && last->next) {
		last = last->next;
	}
	if (last) {
		last->next = h;
	} else {
		filter->func = h;
	}
}

void bloom_add(bloom_t filter, const char *item) {
	struct bloom_hash *h = filter->func;
	uint8_t *bits = filter->bits;
	while (h) {
		uint32_t hash = h->func(item);
		hash %= filter->size * 8;
		bits[hash / 8] |= 1 << hash % 8;
		h = h->next;
	}
}

bool bloom_test(bloom_t filter, const char *item) {
	struct bloom_hash *h = filter->func;
	uint8_t *bits = filter->bits;
	while (h) {
		uint32_t hash = h->func(item);
		hash %= filter->size * 8;
		if (!(bits[hash / 8] & 1 << hash % 8)) {
			return false;
		}
		h = h->next;
	}
	return true;
}
