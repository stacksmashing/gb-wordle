
#include "word-db.h"

word_number_t from_b26(char word[const WORD_LENGTH+1]) {
    word_number_t res = 0;
    word_number_t pow = 1;
    for(uint8_t i = WORD_LENGTH; i > 0; i--) {
        word_number_t digit = word[i-1]-'A';
        res += digit*pow;
        pow *= 26;
    }

    return res;
}

void to_b26(word_number_t word_number, char word[WORD_LENGTH+1]) {
    uint8_t i = 0;
    while(word_number > 0) {
        word[WORD_LENGTH - 1 - i] = 'A' + (word_number % 26);
        word_number /= 26;
        i++;
    }
    for(;i < WORD_LENGTH; i++) {
        word[WORD_LENGTH - 1 - i] = 'A';
    }
}

bool query_word(char word[const WORD_LENGTH+1]) {
    if(true == query_word_compressed(word_guesses, guesses_index, word)) {
        return true;
    }
    if(true == query_word_compressed(word_answers, answers_index, word)) {
        return true;
    }
    return false;
}

void add_unpack_varint(const uint8_t* const varint, word_number_t *current_word, wordlist_size_t *current_offset) {
    *current_word += 1;
    for(uint8_t i = 0; ; i++) {
        const uint8_t b = varint[(*current_offset)++];
        const uint32_t value = b & 0x7F;
        const uint8_t more = b & 0x80;
        *current_word += value << (7*i);
        if(more == 0) {
            break;
        } 
    }
}

bool query_word_compressed(const uint8_t* const words, const index_entry* const index, char word[const WORD_LENGTH+1]) {
    // TODO: permutation optimization
    const word_number_t word_number = from_b26(word);
    const word_index_size_t bucket_idx = word[0] - 'A';
    const index_entry *bucket = &index[bucket_idx];
    const index_entry *bucket_end = &index[bucket_idx+1];

    if(bucket->start_word == word_number) {
        return true;
    }

    word_number_t current_word = bucket->start_word;
    wordlist_size_t current_offset = bucket->offset;
    while(current_offset < bucket_end->offset) {
        add_unpack_varint(words, &current_word, &current_offset);
        if(word_number == current_word) {
            return true;
        }
    }
    return false;
}

void get_word(wordlist_size_t index, char word[WORD_LENGTH+1]) {
    get_word_compressed(word_answers, index, word);
}

void get_word_compressed(const uint8_t* const words, wordlist_size_t index, char word[const WORD_LENGTH+1]) {
    word_number_t current_word = 0;
    wordlist_size_t current_offset = 0;
    for(wordlist_size_t i = 0; i <= index; i++) {
        add_unpack_varint(words, &current_word, &current_offset);
    }

    to_b26(current_word, word);
}
