#include <stdio.h>
#include <string.h>

#include "../word-db.h"

int main() {
    char word1[6] = "";
    char word2[6] = "";
    strncpy(word1, "SOARE", sizeof(word1));
    uint32_t test1 = from_b26(word1);
    to_b26(test1, word2);
    printf("Word: %s -> %x -> %s\n", word1, test1, word2);


    get_word(0, word1);
    printf("Word[0]: %s\n", word1);

    printf("SOARE: %d\n", query_word("SOARE"));
    printf("ABACK: %d\n", query_word("ABACK"));
    printf("SOARE: %d\n", query_word("SOARE"));
    printf("XXXXX: %d\n", query_word("XXXXX"));
    printf("AAHED: %d\n", query_word("AAHED"));
    printf("AAHEF: %d\n", query_word("AAHEF"));
    printf("ZYMIC: %d\n", query_word("ZYMIC"));
    printf("ZYMIB: %d\n", query_word("ZYMIB"));
    printf("BEADS: %d\n", query_word("BEADS"));

    return 0;
}
