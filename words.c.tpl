#include "word-db.h"

const wordlist_size_t word_answers_size = 1;
const uint8_t word_answers[] = { 0 };
const word_index_size_t answers_index_size = 1;
const index_entry answers_index[] = { 
    { .offset = 0, .start_word = 0 }
};

const wordlist_size_t word_guesses_size = 1;
const uint8_t word_guesses[] = { 0 };
const word_index_size_t guesses_index_size = 1;
extern const index_entry guesses_index[] = { 
    { .offset = 0, .start_word = 0 }
};

