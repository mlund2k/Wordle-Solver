//Used for exit constants mostly
#include <stdlib.h>
//printf and puts
#include <stdio.h>
//string comparison functions
#include <string.h>
//Used to seed the rng for random games
#include <time.h>

//Length of a word.
//Note: You cannot change this without changing the word list too.
//You also must adapt the scanf calls for the new length
#define WORD_LENGTH 5
//Number of tries allowed to find the word
#define MAX_TRIES 100
//Number of characters in the alphabet + 1
#define ALPHA_SIZE 27
//If set, the word and a few stats are printed before the game starts
//#define DEBUG

//Very cheap error termination script
#define err(x) fprintf(stderr, EOL "[%s:%i] Fatal error: %s" EOL, __FILE__, __LINE__, x);abort();

//Note: CRLF is also used for telnet.
//If you want to make it available in a BBS you may want to force a CRLF
#ifdef WIN32
#define EOL "\r\n"
#else
#define EOL "\n"
#endif

//Cheap boolean
#define bool int
#define false (0)
#define true (!false)

//Files for lists that contain all words and solutions
FILE * fpAll,  * fpSol, * sol;
//Number of words in the solution list
long wordCount = 0;
//Selected word from solution list
char word[WORD_LENGTH + 1] = {0};
//Possible characters (used to show unused characters)
//The size specifier is necessary or its value will be readonly
char alpha[ALPHA_SIZE] = "abcdefghijklmnopqrstuvwxyz";
//Memory cache:
//0-25 File position in the complete list with words that start with the given letter a-z
//26: Number of words in the solution list
long memcache[ALPHA_SIZE];

//Reads solution list and picks a random word
long setup(void);
//Pick the given word
int pickWord(char * word, int index);
//Checks if the supplied word is in the list
bool hasWord(const char * word);
//Convert to lowercase
int toLower(char * str);
//Checks the entered word against the solution
bool checkWord(const char * guess);
//Checks if the entered string is a potentially valid word
bool isWord(const char* word);
//Gets the first position of the given char in the given string
int strpos(const char * str, char search);
//Removes characters in the supplied argument from the alphabet
void removeAlpha(const char * guess);
//Runs the main game loop
void gameLoop(void);
//Runs the menu
int menu(void);
//Shows the help text
void help(void);

//Main function
int main() {
	int gameId;
	setbuf(stdout, NULL);
	//Note: This will search for the file in the current directory
	fpAll = fopen("LISTS/ALL.TXT", "r");
	fpSol = fopen("LISTS/SOLUTION.TXT", "r");
	if (fpAll == NULL || fpSol == NULL) {
		err("error opening wordle lists");
	}
	#ifdef DEBUG
	printf("Word count: %i" EOL, setup());
	#else
	setup();
	#endif
	gameId = menu();
	if (gameId >= 0) {
		pickWord(word, gameId);
		#ifdef DEBUG
		printf("Word: %s" EOL, word);
		#endif
	} else {
		return EXIT_SUCCESS;
	}
	printf("Running game #%i" EOL, gameId + 1);
	gameLoop();
	fclose(fpAll);
	fclose(fpSol);
	return EXIT_SUCCESS;
}

int menu() {
	char buffer[21];
	int gameId = 0;
	int scan = 0;
	puts(
		"Main Menu" EOL
		"=========" EOL
		"NEW: start a new game." EOL
		"LOAD <num>: load a specific game" EOL
		"HELP: More information" EOL
		"EXIT: End game");
	printf("The game number must be in range of 1-%li" EOL, wordCount);
	while (true) {
		printf("Input: ");
		while ((scan = scanf("%20s", buffer)) != 1) {
			if (scan == EOF) {
				return -1;
			}
		}
		toLower(buffer);
		if (strcmp(buffer, "exit") == 0) {
			return -1;
		} else if (strcmp(buffer, "help") == 0) {
			help();
		} else if (strcmp(buffer, "new") == 0) {
			return rand() % wordCount;
		} else if (strcmp(buffer, "load") == 0) {
			if (scanf("%i",  & gameId) == 1) {
				if (gameId > 0 && gameId <= wordCount) {
					return gameId - 1;
				}
			}
			puts("Invalid number");
		} else {
			puts("Invalid input");
		}
	}
}

void help() {
	printf("Wordle is a simple game:" EOL "Guess the %i letter word within %i tries" EOL, WORD_LENGTH, MAX_TRIES);
	puts(
		"After every guess, hints are shown for each character." EOL
		"They look like this:" EOL
		"  _ = Character not found at all" EOL
		"  # = Character found and position correct" EOL
		"  o = Character found but position wrong" EOL
		"Unused letters of the alphabet are shown next to the hint" EOL
		EOL
		"The game prefers valid positions over invalid positions," EOL
		"And it handles double letters properly." EOL
		"Guessing \"RATES\" when the word is \"TESTS\" shows \"__oo#\"");
}

void gameLoop() {
	
	// **BEGINNING OF EDITED CODE**

	char guess[WORD_LENGTH + 1] = {0};
	//array of correct letters guessed in correct positions
	char correct[WORD_LENGTH + 1] = {'_','_','_','_','_'};
	//array of correct letters guessed in wrong positions
	char misplaced[WORD_LENGTH + 1] = {0};
	//array of wrong letters (not in solution word)
	char wrong[ALPHA_SIZE] = {0};
	//array to read line by line through files for potential guesses
	char line[WORD_LENGTH];
	//array to store potential guesses
	char guesslist[2315][WORD_LENGTH+2];
	//used to keep track of number of potential guesses
	int g = 0;
	int guesses = 0;
	int scan = 0;
	//used for keeping track of misplaced letters
	int k = 0;
	//used for keeping track of wrong letters
	int l = 0;
	puts(
		"word\tunused alphabet" EOL
		"====\t===============");
	//Starts with 3 words that target all vowels and other popular letters:
	//(It is not possible to write a 5 letter English word without one of these characters.)
	while (guesses < MAX_TRIES && strcmp(guess, word)) {
		printf("Guess %i: ", guesses + 1);
		if(guesses < 3){
			switch(guesses){
			//guess 1
			case 0:
				//set guess to first word
				strcpy(guess, "guide");
				printf("%5s\n",guess);
				//iterate through each letter of guess
				for(int i=0;i<WORD_LENGTH;i++){
					//if a letter in guess matches a letter in same index of solution,
					//the letter is stored in the appropriate index of correct, and
					//also added to misplaced correct letters for efficiency
					if(guess[i]==word[i]){
						correct[i] = guess[i];
						misplaced[k] = guess[i];
						//This counter increments ever time a character is written to
						//misplaced to prevent overwriting
						k++;
					}
					//if the indexed letter is not correct, it could still be somewhere in
					//the word, so we need to test more
					else if(guess[i]!=word[i]){
						//for each letter in guess, iterate through every letter of solution
						for(int j=0;j<WORD_LENGTH;j++){
							//if the letter is in fact somewhere in the word outside of the
							//index of guess, it is written to misplaced
							if( (i!=j) && (guess[i]==word[j]) && (correct[i] != guess[i])){
								misplaced[k] = guess[i];
								k++;
								break;
							}
							//if the letter is nowhere in the word, it is written to wrong
							else if((j == WORD_LENGTH-1) && (guess[i]!=word[j])){
								wrong[l] = guess[i];
								//Counter to prevent overwriting in wrong character array
								l++;
							}
						}
					}
				}
				break;
			//guess 2
			case 1:
            	//set guess to second word
				strcpy(guess, "thorn");
				printf("%5s\n",guess);
				//iterate through each letter of guess
				for(int i=0;i<WORD_LENGTH;i++){
					//if a letter in guess matches a letter in same index of solution,
					//the letter is stored in the appropriate index of correct, and
					//also added to misplaced correct letters for efficiency
					if(guess[i]==word[i]){
						correct[i] = guess[i];
						misplaced[k] = guess[i];
						k++;
					}
					//if the indexed letter is not correct, it could still be somewhere in
					//the word, so we need to test more
					else if(guess[i]!=word[i]){
						//for each letter in guess, iterate through every letter of solution
						for(int j=0;j<WORD_LENGTH;j++){
							//if the letter is in fact somewhere in the word outside of the
							//index of guess, it is written to misplaced
							if( (i!=j) && (guess[i]==word[j]) && (correct[i] != guess[i])){
								misplaced[k] = guess[i];
								k++;
								break;
							}
							//if the letter is nowhere in the word, it is written to wrong
							else if((j == WORD_LENGTH-1) && (guess[i]!=word[j])){
								wrong[l] = guess[i];
								l++;
							}
						}
					}
				}
            	break;
        	//guess 3
			case 2:
            	//set guess to third word
				strcpy(guess, "scaly");
				printf("%5s\n",guess);
				//iterate through each letter of guess
				for(int i=0;i<WORD_LENGTH;i++){
					//if a letter in guess matches a letter in same index of solution,
					//the letter is stored in the appropriate index of correct, and
					//also added to misplaced correct letters for efficiency
					if(guess[i]==word[i]){
						correct[i] = guess[i];
						misplaced[k] = guess[i];
						k++;
					}
					//if the indexed letter is not correct, it could still be somewhere in
					//the word, so we need to test more:
					else if(guess[i]!=word[i]){
						//for each letter in guess, iterate through every letter of solution
						for(int j=0;j<WORD_LENGTH;j++){
							//if the letter is in fact somewhere in the word outside of the
							//index of guess, it is written to misplaced
							if( (i!=j) && (guess[i]==word[j]) && (correct[i] != guess[i])){
								misplaced[k] = guess[i];
								k++;
								break;
							}
							//if the letter is nowhere in the word, it is written to wrong
							else if((j == WORD_LENGTH-1) && (guess[i]!=word[j])){
								wrong[l] = guess[i];
								l++;
							}
						}
					}
				}
				break;
        	default:
            	break;
			}
		}
		else if(guesses >= 3){
			//Displays all known data regarding solution word after 3 guesses to user
			if(guesses==3){
				printf("\n\nTop secret computer knowledge:\nIncluded Letters: %s",misplaced);
				printf("\nCorrect Letters: %s",correct);
				printf("\nIncorrect Letters: %s\n\n",wrong);
			}
			//Opening ALL.TXT to test for potential solutions
			sol = fopen("LISTS/ALL.TXT", "r");
			if(sol == NULL){
				err("error opening word list");
			}
			//This loops through the file line by line, interating before each whitespace character
			while(fgets(line,WORD_LENGTH+1,sol)){
				//Some counters to help test words in all.txt
				int a = 0; // number of correct letters
				int b = 0; // number of file line letters that match correct
				int c = 0; // number of misplaced letters in file line
				int d = strlen(wrong); // number of wrong letters in file line
				if(strlen(line)==WORD_LENGTH){
					//looping through each character of given line
					for(int i=0;i<WORD_LENGTH;i++){
						//checking each correct character that is not empty
						if(correct[i] != '_'){
							//counter that increments for every letter in correct
							a++;
							if(line[i] == correct[i]){
								//counter that increments every time the index of given line
								//matches the character in the index of correct
								b++;
							}
							//If the given line is missing a correct letter, forget it and move on
							else{
								break;
							}
						}
					}
					//if a=b, then the given line contains every single correct letter in the correct spot
					//now we will make sure it contains every misplaced letter:
					if(a==b){
						//looping through each misplaced letter
						for(int j=0;j<strlen(misplaced);j++){
							for(int m=0;m<WORD_LENGTH;m++){
								//each misplaced letter is compared to the letters in the word
								if(line[m]==misplaced[j]){
									//if the misplaced letter is found in the word, increment c
									c++;
									break;
								}
							}
						}
					}
					//if c is equal to the size of the misplaced letters, then every
					//misplaced letter was found in the word.
					//time to test for words that don't contain wrong letters:
					if(c==strlen(misplaced)){
						//looping over every incorrect letter
						for(int i=0;i<strlen(wrong);i++){
							for(int j=0;j<WORD_LENGTH;j++){
								//if an incorrect letter is found anywhere in the potential word
								//the word is disregarded as being incorrect
								if(line[j] == wrong[i]){
									//d is initialized at the size of wrong, and decremented
									//every time a wrong letter is found 
									d--;
								}
							}
						}
						//if counter d is not changed, then the word does not contain a wrong letter,
						//has each correct letter in the right spot, and contains every misplaced
						//correct letter, so it is then written to a list for storage:
						if((d==strlen(wrong)) && (strlen(line) == WORD_LENGTH)){
							strcpy(guesslist[g], line);
							//another incrementor to avoid overwriting
							g++;
						}
					}
				}
				
				
			}
			fclose(sol);
			//double checking that each solution is correct length, guesses are then
			//pulled one by one from the 2d character array for each subsequent
			//guess until the game is won
			if(strlen(guesslist[guesses-3])==WORD_LENGTH){
				strcpy(guess, guesslist[guesses-3]);
			}
			//exit code in case a nullspace character is passed
			else{
				err("Guess incorrect length :(");
			}
			printf("%5s\n",guess);
		}
		
		// **END OF EDITED CODE**


		if (strlen(guess) == WORD_LENGTH) {
			toLower(guess);
			//Do not bother doing all the test logic if we've found the word.
			if (strcmp(guess, word)) {
				if (isWord(guess) && hasWord(guess)) {
					++guesses;
					//TODO: Check guess against word
					if (checkWord(guess)) {
						removeAlpha(guess);
						printf("\t%s\n", alpha);
					}
				} else {
					puts("Word not in list");
				}
			}
		} else {
			if (scan == EOF) {
				exit(EXIT_FAILURE);
			}
			printf("Invalid word. Must be %i characters\n", WORD_LENGTH);
		}
	}
	if (strcmp(guess, word)) {
		printf("You lose. The word was %s\n", word);
	} else {
		puts("You win");
	}
}

void removeAlpha(const char * guess) {
	int i = 0;
	int pos = 0;
	if (guess != NULL) {
		while (guess[i]) {
			pos = strpos(alpha, guess[i]);
			if (pos >= 0) {
				alpha[pos] = '_';
			}
			++i;
		}
	}
}

int strpos(const char * str, char search) {
	int i = 0;
	if (str != NULL) {
		while (str[i]) {
			if (str[i] == search) {
				return i;
			}
			++i;
		}
	}
	return -1;
}

bool checkWord(const char * guess) {
	if (strlen(guess) == strlen(word)) {
		int i = 0;
		int pos = -1;
		//Copy is used to blank found characters
		//This avoids wrong reports for double letters, for example "l" in "balls"
		char copy[WORD_LENGTH + 1];
		char result[WORD_LENGTH + 1];
		result[WORD_LENGTH] = 0;
		strcpy(copy, word);
		//Do all correct positions first
		while (copy[i]) {
			if (copy[i] == guess[i]) {
				//Character found and position correct
				result[i] = '#';
				copy[i] = '_';
			} else {
				//Fills remaining slots with blanks
				//We could do this before the loop as well
				result[i] = '_';
			}
			++i;
		}
		i = 0;
		while (copy[i]) {
			pos = strpos(copy, guess[i]);
			//Char must exist but do not overwrite a good guess
			if (pos >= 0 && result[i] != '#') {
				//Character found but position wrong
				result[i] = 'o';
				copy[pos] = '_';
			}
			++i;
		}
		printf("%s", result);
		return true;
	}
	return false;
}

int toLower(char * str) {
	int i = 0;
	while (str[i]) {
		if (str[i] >= 'A' && str[i] <= 'Z') {
			str[i] |= 0x20; //Make lowercase
		}
		++i;
	}
	return i;
}

int hasWord(const char * word) {
	//A bit longer to also contain the line terminator
	char buffer[WORD_LENGTH + 4];
	//Don't bother if the argument is invalid
	if (word == NULL || strlen(word) != WORD_LENGTH || !isWord(word)) {
		return false;
	}
	fseek(fpAll, memcache[word[0]-'a'], SEEK_SET);
	//Stop comparing once we are beyond the current letter
	printf("buffer 0 %c .",buffer[0]);
	printf("word 0 %c .",word[0]);
	while (fgets(buffer, WORD_LENGTH + 4, fpAll) != NULL && buffer[0]==word[0]) {
		buffer[WORD_LENGTH]=0;
		if (strcmp(word, buffer) == 0) {
			printf("hasWord all good");
			return true;
		}
	}
	printf("hasWord NOT good2");
	return false;
}

bool isWord(const char* word){
	int i=-1;
	if(strlen(word) == WORD_LENGTH){
		while(word[++i]){
			if(word[i]<'a' || word[i]>'z'){
				return false;
			}
		}
		return true;
	}
	return false;
}

long setup() {
	FILE* cache;
	char currentChar;
	char currentWord[WORD_LENGTH + 1];
	bool success = false;
	
	//Don't bother if setup was already performed
	if (wordCount > 0) {
		return wordCount;
	}
	srand((int)time(0));
	
	if ((cache = fopen("LISTS\\CACHE.BIN","rb")) != NULL) {
		printf("Reading cache... ");
	    success = fread(memcache, sizeof(long), ALPHA_SIZE, cache) == ALPHA_SIZE;
	    fclose(cache);
		if(success){
			puts(" [OK]");
			return wordCount = memcache[ALPHA_SIZE - 1];
		}
		else{
			puts(" [FAIL]");
		}
	}
	
	printf("Loading word list...");
	fseek(fpSol, 0, SEEK_SET);
	while (fgets(currentWord, WORD_LENGTH + 1, fpSol) != NULL) {
		//Only increment if word length is correct
		if (strlen(currentWord) == WORD_LENGTH) {
			++wordCount;
		}
	}
	puts(" [OK]");
	memcache[ALPHA_SIZE-1] = wordCount;

	if (!success) {
	    printf("Building cache...");
		currentChar = 'a';
		memcache[0] = 0;
		fseek(fpAll, 0, SEEK_SET);
		while (fgets(currentWord, WORD_LENGTH + 1, fpAll) != NULL) {
			//Only proceed if word length is correct
			if (strlen(currentWord) == WORD_LENGTH) {
			    if (currentChar != currentWord[0]) {
			        currentChar = currentWord[0];
			        memcache[currentChar - 'a'] = ftell(fpAll) - 5;
			    }
			}
		}
		cache = fopen("LISTS\\CACHE.BIN", "wb");
		if (cache == NULL) {
			puts(" [FAIL]");
		}
		else{
			fwrite(memcache, sizeof(long), ALPHA_SIZE, cache);
			fclose(cache);
			puts(" [OK]");
		}
	}

	return wordCount;
}

int pickWord(char * word, int index) {
	int i = 0;
	fseek(fpSol, 0, SEEK_SET);
	while (i <= index && fgets(word, WORD_LENGTH + 1, fpSol) != NULL) {
		if (strlen(word) == WORD_LENGTH) {
			++i;
		}
	}
	return index;
}
