/* Wordle for Game Boy.
 * A weekend project by stacksmashing.
 * Contact: twitter.com/ghidraninja
 *          code@stacksmashing.net
 */
#include <gb/gb.h>
#include <gb/drawing.h>

#include <gbdk/console.h>
#include <gbdk/font.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <rand.h>

#include "word-db.h"

#define MAX_GUESSES 6

void set_box_color_for_letter(char *word, int position, char letter);
void set_letter_color_for_letter(char *word, int position, char letter);

const char *kb[3] = {
"Q W E R T Y U I O P",
" A S D F G H J K L",
"  Z X C V B N M"};

int kb_coords[3] = {
    10,
    9,
    7
};

int kb_offsets[3] = {
    0,
    1,
    2
};

int kb_x = 0;
int kb_y = 0;
int guess_nr;
char guess[WORD_LENGTH+1];
char guesses[WORD_LENGTH+1][MAX_GUESSES];
char guessed_wrong[30];
char guessed_position[30];
char guessed_correct[30];
char word[WORD_LENGTH+1];

void draw_word_rect(int x, int y, char *guess) {
    int gx = x/8;
    int gy = y/8;
    x -= 3;
    y -= 4;
    for(int i=0; i < 5; i++) {
        if(guess) {
            char letter = guess[i];
            set_box_color_for_letter(word, i, letter);
            box(x, y, x+14, y+14, M_FILL);
            set_letter_color_for_letter(word, i, letter);
            gotogxy(gx, gy);
            wrtchr(letter);
            gx += 2;
        } else {
            color(BLACK, WHITE, M_NOFILL);
            box(x, y, x+14, y+14, M_NOFILL);
        }
        
        x += 16;
    }
}


int contains(char *str, char c) {
    int l = strlen(str);
    for(int i=0; i < l; i++) {
        if(str[i] == c) {
            return 1;
        } 
    }
    return 0;
}



void set_box_color_for_letter(char *word, int position, char letter) {
    if(word[position] == letter) {
        color(BLACK, BLACK, M_FILL);
    } else if(contains(word, letter)) {
        color(BLACK, DKGREY, M_FILL);
    } else {
        color(BLACK, WHITE, M_NOFILL);
    }
}

void set_letter_color_for_letter(char *word, int position, char letter) {
    if(word[position] == letter) {
        color(WHITE, BLACK, M_NOFILL);
    } else if(contains(word, letter)) {
        color(WHITE, DKGREY, M_NOFILL);
    } else {
        color(BLACK, WHITE, M_NOFILL);
    }
}

void set_color_for_gletter(char letter) {
    if(letter == ' ') {
        color(BLACK, WHITE, M_NOFILL);
    } else if (contains(guessed_wrong, letter)) {
        color(BLACK, WHITE, M_NOFILL);
    } else if(contains(guessed_position, letter)) {
        color(WHITE, WHITE, M_NOFILL);
    } else if(contains(guessed_correct, letter)) {
        color(WHITE, WHITE, M_NOFILL);
    } else {
        color(BLACK, WHITE, M_NOFILL);
    }
}


void set_color_for_letter(char letter) {
    if(letter == ' ') {
        color(BLACK, WHITE, M_NOFILL);
    } else if (contains(guessed_wrong, letter)) {
        color(LTGREY, WHITE, M_NOFILL);
    } else if(contains(guessed_position, letter)) {
        color(DKGREY, WHITE, M_NOFILL);
    } else if(contains(guessed_correct, letter)) {
        color(DKGREY, WHITE, M_NOFILL);
    } else {
        color(BLACK, WHITE, M_NOFILL);
    }
}

void draw_keyboard(int x, int y) {
    for(int i=0; i < 3; i++) {
        gotogxy(x, y + i);
        int kbl = strlen(kb[i]);
        for(int j=0; j < kbl; j++) {
            char letter = kb[i][j];
            set_color_for_letter(letter);
            wrtchr(letter);
        }
    }
}

int kb_vert_offset = 14;

void highlight_key() {
    int x = (kb_x * 16) + (kb_offsets[kb_y] * 8);
    int y = (kb_vert_offset + kb_y) * 8;
    color(BLACK, WHITE, M_NOFILL);
    box(x, y-1, x+8, y+7, M_NOFILL);
}

void dehighlight_key() {
    int x = (kb_x * 16) + (kb_offsets[kb_y] * 8);
    int y = (kb_vert_offset + kb_y) * 8;
    color(WHITE, WHITE, M_NOFILL);
    box(x, y-1, x+8, y+7, M_NOFILL);


    int gx = (kb_x * 2) + (kb_offsets[kb_y]);
    int gy = (kb_vert_offset + kb_y);
    gotogxy(gx, gy);
    char letter = kb[kb_y][kb_offsets[kb_y]+ (kb_x*2)];
    set_color_for_letter(letter);
    wrtchr(letter);
}

char getletter() {
    return kb[kb_y][kb_offsets[kb_y] + (kb_x*2)];
}

void render_guess() {
    // first box is at 5, 2
    int line = 2 + (guess_nr * 2);
    int x = 5;
    for(int i=0; i < 5; i++) {
        color(BLACK, WHITE, M_NOFILL);
        gotogxy(x, line);
        if(guess[i] != 0) {
            wrtchr(guess[i]);
        } else {
            wrtchr(' ');
        }

        x += 2;
    }
}

void draw_board() {
    for(int i=0; i < MAX_GUESSES; i++) {
        char *g = NULL;
        if(i < guess_nr) {
            g = guesses[i];
        }
        draw_word_rect(40, 16+(i*16), g);
    }

}


void show_win() {
    
    color(BLACK, BLACK, M_FILL);
    box(0, 0, SCREENWIDTH, SCREENHEIGHT, M_FILL);
    gotogxy(0, 8);
    color(WHITE, BLACK, M_NOFILL);
    gprint("     You won!!!");
    gotogxy(0, 9);
    gprintf("   %d/6 - Congrats!", guess_nr);
    waitpad(J_START | J_A);
    reset();
}

void show_loose() {
    // cls();
    color(BLACK, BLACK, M_FILL);
    box(0, 0, SCREENWIDTH, SCREENHEIGHT, M_FILL);
    gotogxy(0, 8);
    color(WHITE, BLACK, M_NOFILL);
    gprint(" You lost. Sorry!");
    waitpad(J_START | J_A);
    reset();
}


void analyze_guess(char *guess) {
    for(int i = 0; i < WORD_LENGTH; i++) {
        if(guess[i] == word[i]) {
            guessed_correct[strlen(guessed_correct)] = guess[i];
        } else if(contains(word, guess[i])) {
            guessed_position[strlen(guessed_position)] = guess[i];
        } else {
            guessed_wrong[strlen(guessed_wrong)] = guess[i];
        }
    }
}

void run_wordle(void)
{
    strcpy(word, "EMPTY");
    int has_random = 0;
    
    guess_nr = 0;
    memset(guess, 0, sizeof(guess));
    memset(guessed_wrong, 0, sizeof(guessed_wrong));
    memset(guessed_position, 0, sizeof(guessed_position));
    memset(guessed_correct, 0, sizeof(guessed_correct));

    for(int i=0; i < MAX_GUESSES; i++) {
        strcpy(guesses[i], "");
    }
    for(int i=0; i < MAX_GUESSES; i++) {
        draw_word_rect(40, 16+(i*16), NULL);
    }

    gotogxy(2, 0);
    gprint("GameBoy  WORDLE");
    draw_keyboard(0, kb_vert_offset);

    color(LTGREY, WHITE, M_NOFILL);
    highlight_key();
    while(1) {
        int j = joypad();
        if((has_random == 0) && (j != 0)) {
            uint16_t seed = LY_REG;
            seed |= (uint16_t)DIV_REG << 8;
            initrand(seed);
            int r = rand();
            while(r > 211) {
                r = rand();
            }
            get_word(r, word);
            has_random = 1;
        }

        switch(j) {
            case J_RIGHT:
                dehighlight_key();
                kb_x += 1;
                if(kb_x >= kb_coords[kb_y]) {
                    kb_x = 0;
                }
                highlight_key();
                waitpadup();
                break;
            case J_LEFT:
                dehighlight_key();
                kb_x -= 1;
                if(kb_x < 0) {
                    kb_x = kb_coords[kb_y] - 1;
                }
                highlight_key();
                waitpadup();
                break;
            case J_UP:
                dehighlight_key();
                kb_y -= 1;
                if(kb_y < 0) {
                    kb_y = 2;
                }
                if(kb_x >= kb_coords[kb_y]) {
                    kb_x = kb_coords[kb_y] - 1;
                }
                highlight_key();
                waitpadup();
                break;
            case J_DOWN:
                dehighlight_key();
                kb_y += 1;
                if(kb_y > 2) {
                    kb_y = 0;
                }
                if(kb_x >= kb_coords[kb_y]) {
                    kb_x = kb_coords[kb_y] - 1;
                }
                highlight_key();
                waitpadup();
                break;
            case J_SELECT:
            case J_START:
                if(strlen(guess) != WORD_LENGTH) break;
                if(!query_word(guess)) break;
                analyze_guess(guess);
                strcpy(guesses[guess_nr], guess);
                guess_nr += 1;
                draw_board();
                draw_keyboard(0, kb_vert_offset);
                highlight_key();
                if(strcmp(word, guess) == 0) {
                    show_win();
                    return;
                    break;
                }
                if(guess_nr == MAX_GUESSES) {
                    show_loose();
                    return;
                    break;
                }
                // empty guess
                memset(guess, 0, 5);
                // TODO
                break;
            case J_A:
                if(strlen(guess) == WORD_LENGTH) break;
                guess[strlen(guess)] = getletter();
                render_guess();
                waitpadup();
                break;
            case J_B:
                if(strlen(guess) == 0) break;
                guess[strlen(guess)-1] = 0;
                render_guess();
                waitpadup();
                break;
            default:
            break;
        }

        wait_vbl_done();
    }
}

void main() {
    while(1) {
        run_wordle();    
    }
}
