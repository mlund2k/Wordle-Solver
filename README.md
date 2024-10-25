# Wordle-Solver


My personal attempt at creating a solving algorithm for the popular word game Wordle.


This program was compiled using WSL (Windows Subsystem for Linux) on Windows 10 and 11 and runs perfectly in such environments.
This program may have issues running properly through other means, and I ran into issues running correctly through Visual Studio Code due to a problem I am aware of but not knowledgeable to solve.
The problem that causes this has to do with how the files in the directory are pathed, namely ALL.TXT and SOLUTION.TXT in the "lists" folder. In order to bypass this problem on any particular operating system, lines 79, 80, and 314 must be edited to accomadate the desired file path system, for instance:


```sol = fopen("LISTS/ALL.TXT", "r");```


may need to become


```sol = fopen("LISTS//ALL.TXT", "r");```


on some operating systems.


The included program (lund_wordle.c) contains the program code.
The framework of this wordle works the same to the original unmodified file, however now each game is completed by a computer.

The only edits made to the original wordle file are an increase in the
MAX_TRIES global variable and additional code in the gameloop() function
to replace the previously used user input with computer generated responses.
An explanation of the changes is included below. Plenty of comments are
interspersed throughout the code.

To play the game, run the makefile in your terminal (WSL or other environment with the respective edited file paths), enter "./wordle" to run the executable, and either load a specific line number
from SOLUTION.TXT using "line <number here>", or randomly select one by typing "new".
The game will then automatically play out until the computer finds the correct word.


Gameloop() Algorithm Function Explained:

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
