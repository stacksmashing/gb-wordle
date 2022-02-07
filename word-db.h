#include <stdint.h>
#include <stdbool.h>

#define WORD_LENGTH 5

typedef uint32_t word_number_t;
typedef uint16_t wordlist_size_t;
typedef uint8_t word_index_size_t;
typedef struct {
    word_number_t start_word;
    wordlist_size_t offset;
} index_entry;

extern const wordlist_size_t word_answers_size;
extern const uint8_t word_answers[];
extern const word_index_size_t answers_index_size;
extern const index_entry answers_index[];

extern const wordlist_size_t word_guesses_size;
extern const uint8_t word_guesses[];
extern const word_index_size_t guesses_index_size;
extern const index_entry guesses_index[];

// public
bool query_word(char word[const WORD_LENGTH+1]);
void get_word(wordlist_size_t index, char word[WORD_LENGTH+1]);

// private
bool query_word_compressed(const uint8_t* const words, const index_entry* const index, char word[const WORD_LENGTH+1]);
void get_word_compressed(const uint8_t* const words, wordlist_size_t index, char word[const WORD_LENGTH+1]);

word_number_t from_b26(char word[const WORD_LENGTH+1]);
void to_b26(word_number_t word_number, char word[WORD_LENGTH+1]);
