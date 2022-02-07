#
# Simple Makefile that compiles all .c and .s files in the same folder
#

# If you move this project you can change the directory 
# to match your GBDK root directory (ex: GBDK_HOME = "C:/GBDK/"
GBDK_HOME = /home/zetatwo/tools/gbdk-2020/build/gbdk/

LCC = $(GBDK_HOME)bin/lcc -Wa-l -Wl-m -Wl-j

# You can uncomment the line below to turn on debug output
# LCC = $(LCC) -debug

# You can set the name of the .gb ROM file here
PROJECTNAME    = WORDLE

BINS	    = $(PROJECTNAME).gb
CSOURCES   := main.c word-db.c words.c
ASMSOURCES := $(wildcard *.s)

all:	$(BINS)

words.c: wordle_compress/generate_words.py wordle_compress/wordlist_answers.txt wordle_compress/wordlist_guesses.txt
	python3 wordle_compress/generate_words.py wordle_compress/wordlist_answers.txt wordle_compress/wordlist_guesses.txt words.c

compile.bat: Makefile
	@echo "REM Automatically generated from Makefile" > compile.bat
	@make -sn | sed y/\\//\\\\/ | grep -v make >> compile.bat

# Compile and link all source files in a single call to LCC
$(BINS):	$(CSOURCES) $(ASMSOURCES)
	$(LCC) -o $@ $(CSOURCES) $(ASMSOURCES)

clean:
	rm -f *.o *.lst *.map *.gb *.ihx *.sym *.cdb *.adb *.asm

