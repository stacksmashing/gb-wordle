#!/usr/bin/env python3

import string
import sys

C_FILE_TEMPLATE = """
#include "word-db.h"

const wordlist_size_t word_answers_size = %d;
const uint8_t word_answers[] = { %s };
const word_index_size_t answers_index_size = %d;
const index_entry answers_index[] = {
    //{ .offset = 0, .start_word = 0 }
    %s 
};

const wordlist_size_t word_guesses_size = %d;
const uint8_t word_guesses[] = { %s };
const word_index_size_t guesses_index_size = %d;
const index_entry guesses_index[] = { 
    //{ .offset = 0, .start_word = 0 }
    %s
};
"""

def pack_varint(num):
    """Pack a number as a variable length integer"""
    assert num > 0
    num -= 1
    if num == 0:
        return bytes([0])
    res = []
    while num > 0:
        part = num & 0x7F
        num = num >> 7
        if num > 0:
            part |= 0x80
        res.append(part)
    return bytes(res)

def unpack_varint(data):
    """Unpack a variable length integer into a number"""
    res = 0
    for i, b in enumerate(data):
        value = b & 0x7F
        more = b >> 7
        res += value << (7*i)
        if more == 0:
            break
    return res+1, i+1

def from_b26(word, l=5):
    """Convert a word into a number by treating it as a base 26 number"""
    assert len(word) == l, len(word)
    assert all(x in string.ascii_lowercase for x in word), word
    res = 0
    for i, c in enumerate(word.encode('ascii')[::-1]):
        res += (c-ord('a'))*26**i
    return res

def to_b26(word, l=5):
    """Convert a number into a word by converting it into a base 26 number"""
    digits = []
    while word > 0:
        digits.append(word % 26)
        word = word // 26
    
    return ''.join(string.ascii_lowercase[x] for x in digits[::-1]).rjust(l, 'a')


def compress_wordlist_varlen(words):
    """Compress a list of words into a delta encoded sequence of variable length integers"""
    word_numbers = [from_b26(word) for word in words]
    current = 0
    res = b''
    for word_number in word_numbers:
        delta = word_number - current
        assert delta > 0
        packed = pack_varint(delta)
        res += packed
        current = word_number

    return res
    
def decompress_wordlist_varlen(compressed):
    """Decompress a list of words from a delta encoded sequence of variable length integers"""
    words = []
    current_word = 0
    current = 0

    while current < len(compressed):
        delta, word_len = unpack_varint(compressed[current:])
        current_word += delta
        current += word_len
        words.append(to_b26(current_word))    
    
    return words

def build_index(compressed_words):
    buckets = [(None, None)]*26
    current_word = 0
    current = 0
    prev_bucket_idx = -1
    while current < len(compressed_words):
        delta, word_len = unpack_varint(compressed_words[current:])
        current_word += delta
        current += word_len
        bucket_idx = current_word // 26**4
        if bucket_idx != prev_bucket_idx:
            buckets[bucket_idx] = (current, current_word)
            prev_bucket_idx = bucket_idx
    buckets.append((len(compressed_words), 0))

    for i in range(len(buckets)-1, -1, -1):
        if buckets[i-1][0] == None:
            buckets[i-1] = (buckets[i][0], 0xffffffff)

    return buckets

def main():
    if len(sys.argv) < 4:
        print(f'Usage: {sys.argv[0]} <answers.txt> <guesses.txt> <words.c>')
        return -1

    with open(sys.argv[1], 'r') as fin:
        words1 = sorted(word.strip() for word in fin)
    with open(sys.argv[2], 'r') as fin:
        words2 = sorted(word.strip() for word in fin)

    print(f'Number of answer words: {len(words1)}')
    if len(words1) != 2315:
        print('Warning: Not using the official Wordle wordlist for answers')
    words1_sorted = sorted(words1)
    words1_compressed = compress_wordlist_varlen(words1_sorted)
    print(f'Compressed  size: {len(words1_compressed)}')
    words1_decompressed = decompress_wordlist_varlen(words1_compressed)
    print(f'Decompression correct: {words1_decompressed == words1_sorted}')

    words1_hex = ', '.join(f'{x:#04x}' for x in words1_compressed)
    words1_buckets = build_index(words1_compressed)
    words1_buckets_c = ',\n    '.join(f'{{ .offset = {offset}, .start_word = {start_word:#x} }}' for offset, start_word in words1_buckets)

    print(f'Number of valid guess words: {len(words2)}')
    if len(words2) != 10657:
        print('Warning: Not using the official Wordle wordlist for valid guesses')
    words2_sorted = sorted(words2)
    words2_compressed = compress_wordlist_varlen(words2_sorted)
    print(f'Compressed  size: {len(words2_compressed)}')
    words2_decompressed = decompress_wordlist_varlen(words2_compressed)
    print(f'Decompression correct: {words2_decompressed == words2_sorted}')

    words2_hex = ', '.join(f'{x:#04x}' for x in words2_compressed)
    words2_buckets = build_index(words2_compressed)
    words2_buckets_c = ',\n    '.join(f'{{ .offset = {offset}, .start_word = {start_word:#x} }}' for offset, start_word in words2_buckets)

    words2_hex = ', '.join(f'{x:#04x}' for x in words2_compressed)

    with open(sys.argv[3], 'w') as fout:
        fout.write(C_FILE_TEMPLATE % (
            len(words1_compressed), words1_hex, len(words1_buckets), words1_buckets_c,
            len(words2_compressed), words2_hex, len(words2_buckets), words2_buckets_c,
        ))

if __name__ == '__main__':
    sys.exit(main())
