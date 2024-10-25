The included program (lund_wordle.c) contains the program to run
and solve any wordle in a very limited number of tries.

The framework of wordle works the same to the original unmodified file,
however now each game is completed by a computer.

The only edits made to the original wordle file are an increase in the
MAX_TRIES global variable and additional code in the gameloop() function
to replace the previously used user input with computer generated responses.
An explanation of the changes is included below. Plenty of comments are
interspersed throughout the code.

To play the game, run the makefile in your terminal, enter "./wordle" to run the executable, and either load a specific line number
from SOLUTION.TXT using "line <number here>", or randomly select one by typing "new".
The game will then automatically play out until the computer finds the correct
word.

This program was compiled on a Windows 11 machine running WSL.

Gameloop() Function Explained:

- Character arrays:
In order to accurately filter potential solutions out of ALL.TXT, I have introduced character
arrays to keep track of correct letter placements, correct letters that are misplaced, and
incorrect letters that should not be in the guess. I have also created a line character array,
used to read line by line through ALL.TXT as I test words. All my potential guesses will then
be saved to an array of strings 'guesslist' where I will end up guessing each string thereafter.

- Algorithm:
My algorithm starts by guessing the same 3 words every round: "guide","thorn", and "scaly" in
order to test for every vowel, and 10 more popularly used letters, in fact, it is not possible to 
create a 5 letter english word without one of these letters. Each of these 3 words is then
compared to the solution word internally to determine whether each of its characters have
correct placement, are correct but in the wrong spot, or are not in the word entirely.
Upon creating a profile for the solution word, the algorithm then opens ALL.TXT and scans
through all 10,000+ words to find only words that contain correct letters in the right spots,
contain every misplaced correct letter, and that do not contain a single wrong letter. This process
allows the computer to filter down 10,000 possible results to just a handful at most, and the computer
will often find the word on the 4th guess. After finding every possible word and storing it in a
2d character array, the algorithm then iterates through each row as a potential guess until the
correct word is found and the game is over.