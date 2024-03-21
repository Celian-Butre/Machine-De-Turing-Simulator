#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct stat st = {0};

//STRUCTURE TABLEAU DYNAMIQUE ENTIER

struct Tape{
    int size;
    int* tab;
    int blank;
};

typedef struct Tape Tape;

Tape* createTape(int blank){
    Tape* tabdynTape = malloc(sizeof(Tape));
    tabdynTape->blank = blank;
    tabdynTape->size = 16;
    tabdynTape->tab = malloc(sizeof(int)*16);
    return(tabdynTape);
}



//STRUCTURE TABLEAU DYNAMIQUE STRING

struct dynamicStringTable{
    int size;
    char** tab;
    int numbOfElem;
};

typedef struct dynamicStringTable dynamicStringTable;

dynamicStringTable* createdynamicStringTable(){
    dynamicStringTable* tabdyn = malloc(sizeof(dynamicStringTable));
    tabdyn->size = 16;
    tabdyn->numbOfElem = 0;
    tabdyn->tab = malloc(sizeof(int)*16);
    return(tabdyn);
}

void expanddynamicStringTable(dynamicStringTable* tabdyn){
    tabdyn->size *= 2;
    tabdyn->tab = realloc(tabdyn->tab, sizeof(int)*tabdyn->size);
}


// STRUCTURE NEXTSTEP POUR FAIRE UN TUPLE (nextState, nextDirection, changedChar) = int newState, (-1|0|1), (int newChar) cf turing upgrades 1

struct nextStep{
    int nextState;
    int nextDirection;
    int nextChar;
    bool defined;
};

typedef struct nextStep nextStep;

// STRUCTURE TEMPTRANSITIONS POUR PLUS FACILEMENT INITIALISER UNE MACHINE DE TURING

struct tempTransitions{
    int currentState;
    int currentChar;
    int nextState;
    int nextDirection;
    int nextChar;
};

typedef struct tempTransitions tempTransitions;

// STRUCTURE DE MACHINE DE TURING

struct turing{
    // pour avoir tout du même type on utilise des int pour les char
    Tape* tape;
    // possibilité de les transformer avec 
    char** charConverter;
    // état sous la forme d'un int, aussi possibilité de les transformer avec 
    int currentState;
    char** stateConverter;
    // transitions sous la forme d'un tableau-ception ([currentState][readchar]=nextStep)
    nextStep** transitions;
    // états finals dans un boolswitch
    bool* finalStates;
    // bool qui dit si la machine de turing s'est arrêté
    bool halted;
    // l'indice de la position de la tête (peut-être qu'on changera en un pointeur pour l'arithmétique pointeur)
    int headPosition;
    // symbol blank qui sera celui qui remplit la machine dans les infinits
    int blankChar;
};

typedef struct turing turing;



// ALGOS ÉLÉMENTAIRES POUR UTILISER LA STRUCTURE MACHINE DE TURING


Tape* makeEmptyTape(int blank) {
    Tape* tape = createTape(blank);
    for (int i = 0; i<16; i++) {
        tape->tab[i] = blank;
    }
    return(tape);
};


void expandTapeRight(turing* turingMachine){
    int oldSize = turingMachine->tape->size;
    turingMachine->tape->size *= 2;
    turingMachine->tape->tab = realloc(turingMachine->tape->tab, sizeof(int)*turingMachine->tape->size);
    for (int i = oldSize; i < turingMachine->tape->size; i++) {
        turingMachine->tape->tab[i] = turingMachine->tape->blank;
    }
}

void expandTapeLeft(turing* turingMachine){
    int oldSize = turingMachine->tape->size;
    turingMachine->tape->size *=2;
    int* newTab = malloc(sizeof(int)*turingMachine->tape->size);
    for (int i = 0; i<oldSize; i++) {
        newTab[i] = turingMachine->tape->blank;
    }
    for (int i = 0; i<oldSize; i++) {
        newTab[i+oldSize] = turingMachine->tape->tab[i];
    }
    turingMachine->headPosition += oldSize;
    free(turingMachine->tape->tab);
    turingMachine->tape->tab = newTab;
}

void modifyTape(turing* turingMachine, int newChar, int newCharIndex){
    while (newCharIndex >= turingMachine->tape->size) {
        expandTapeRight(turingMachine);
    }
    turingMachine->tape->tab[newCharIndex] = newChar;
    while (newCharIndex < 0) {
        int oldSize = turingMachine->tape->size;
        expandTapeLeft(turingMachine);
        modifyTape(turingMachine, newChar, newCharIndex + oldSize);
    }
}

void customizeStartingTape(turing* turingMachine, int* startingTab, int startingTabLength) {
    for (int i = 0; i<startingTabLength; i++) {
        modifyTape(turingMachine, startingTab[i], i);
    }
}

turing* makeTuringMachine(/*int* alphabet,*/ int alphabetSize, int blank, /*on ne précise pas les symboles permis d'apparaître au début*/ /*int* states,*/ int stateSize, int starterState, int* finalStates, int finalStatesSize, tempTransitions* transitions, int transitionsSize, int* startingTape, int startingTapeLength, int headPosition/*char* alphabet, int alphabetSize, char blank, /*on ne précise pas les symboles permis d'apparaître au début* / char** states, int stateSize, char* starterState, char** finalStates, int finalStatesSize, tempTransitions* transitions, int transitionsSize */) {
    //FOR NOW BOTH ALPHABET AND STATES ARE INT, CONVERSION WILL COME LATER

    turing* turingMachine = malloc(sizeof(turing)); //allocate turing structure




    // initialise an empty tape
    turingMachine->tape = makeEmptyTape(blank);
    customizeStartingTape(turingMachine, startingTape, startingTapeLength);


    // set initial state
    turingMachine->currentState = starterState;



    turingMachine->transitions = malloc(sizeof(nextStep*)*stateSize);
    for (int i = 0; i<stateSize; i++) {
        turingMachine->transitions[i] = malloc(sizeof(nextStep)*alphabetSize);
        for (int j = 0; j<alphabetSize;j++) {
            turingMachine->transitions[i][j].defined = false;
        }
    }   // allocate transition table

    // fill transition table
    for (int i = 0; i<transitionsSize; i++) {
        
        turingMachine->transitions[transitions[i].currentState][transitions[i].currentChar].nextState =  transitions[i].nextState;
        turingMachine->transitions[transitions[i].currentState][transitions[i].currentChar].nextDirection =  transitions[i].nextDirection;
        turingMachine->transitions[transitions[i].currentState][transitions[i].currentChar].nextChar =  transitions[i].nextChar;
        turingMachine->transitions[transitions[i].currentState][transitions[i].currentChar].defined =  true;
    }


    // make bool switch for final states
    turingMachine->finalStates = malloc(sizeof(bool)*stateSize);
    for (int i = 0; i<stateSize; i++) {
        turingMachine->finalStates[i] = false;
    }
    for (int i = 0; i<finalStatesSize; i++) {
        turingMachine->finalStates[finalStates[i]] = true;
    }


    // set halted to false
    turingMachine->halted = false;


    // address for starting point (as given in input)
    turingMachine->headPosition = headPosition;  // version pointeur : &(turingMachine->tape->tab[headPosition]);


    // set blank character
    turingMachine->blankChar = blank;

    // make int to char converters
    /*
    turingMachine->stateConverter = malloc(sizeof(char*)*stateSize);
    turingMachine->stateConverter[0] = starterState;
    int adjust = 1;
    for (int i = 0; i<stateSize-1; i++) {
        if (strcmp(starterState, states[i-adjust]) == 0) {
            adjust = 0;
        } else {
        turingMachine->stateConverter[i+adjust] = states[i];
        }
    }


    turingMachine->charConverter = malloc(sizeof(char*)*alphabetSize);
    turingMachine->charConverter[0] = blank;
    for (int i = 0; i<alphabetSize-1; i++) {
        if (strcmp(blank, alphabet[i-adjust]) == 0) {
            adjust = 0;
        } else {
        turingMachine->charConverter[i+adjust] = alphabet[i];
        }
    }
    */


    return(turingMachine);
};

void printMachine(turing* turingMachine){
    int screenWidth = 75; // to fill with ------
    printf("Current State : %d\n", turingMachine->currentState);
    for (int i = 0; i < screenWidth; i++) {
        printf("_");
    }
    printf("\n");
    for (int j = 0; j<turingMachine->tape->size; j++) {
        if (turingMachine->headPosition == j) {
            printf("|");
            printf("\033[0;31m");
            printf("%d", turingMachine->tape->tab[j]);
            printf("\033[0m");
        } else {
            printf("|%d", turingMachine->tape->tab[j]);
        }
    }
    printf("|\n");
    for (int i = 0; i < screenWidth; i++) {
        printf("‾");
    }
    printf("\n");
}


void oneStep(turing* turingMachine) {
    // make sure machine is not halted
    if (!turingMachine->halted) { // analyze current + expected transition
        int readCharacter = turingMachine->tape->tab[turingMachine->headPosition];
        int currentState = turingMachine->currentState;
        nextStep next = turingMachine->transitions[currentState][readCharacter];
        if (!next.defined){ // check if transition defined
            turingMachine->halted = true;
        } else {
            turingMachine->currentState = next.nextState;  // update state
            modifyTape(turingMachine, next.nextChar, turingMachine->headPosition); // update tape
            turingMachine->headPosition += next.nextDirection; // update head
            // expand tape if necessary based on head
            if (turingMachine->headPosition == -1) { 
                expandTapeLeft(turingMachine);
            }
            if (turingMachine->headPosition >= turingMachine->tape->size) {
                expandTapeRight(turingMachine);
            }

            // halt if in final state
            if (turingMachine->finalStates[turingMachine->currentState]) {
                turingMachine->halted = true;
            }

        }

    }

}

void runMachine(turing* turingMachine) {
    printMachine(turingMachine);
    while (!turingMachine->halted) {
        oneStep(turingMachine);
        printMachine(turingMachine);
    }
}

struct printParameters {
    int* fileCount;
    char* OutputDir;
    int leftBorderChar;
    int rightBorderChar;
    int printState;
};

typedef struct printParameters printParameters;



void writeMachineToFile(turing* turingMachine, printParameters* printParams) {
    printParams->fileCount[0]++;
    //FILE* ptr = fopen(("/%s/input/%s%d.txt", OutputDir, OutputDir, fileCount),"w");
    /*
    char bufmake[0x200];
    snprintf(bufmake, sizeof(bufmake), "./%s/Input/", printParams->OutputDir);
    if (stat(bufmake, &st) == -1) {
        mkdir(bufmake, 0700);
    } 
    */  
    char buf[0x200];
    snprintf(buf, sizeof(buf), "./%s/Input/%s%d.txt", printParams->OutputDir, printParams->OutputDir, printParams->fileCount[0]);
    FILE* ptr = fopen(buf, "w");
    int customHead = 0;
    while (turingMachine->tape->tab[customHead] != printParams->leftBorderChar) {
        customHead++;
    }
    customHead++;
    while (turingMachine->tape->tab[customHead] != printParams->rightBorderChar) {
        char buf1[0x100];
        snprintf(buf1, sizeof(buf1), "%d\n", turingMachine->tape->tab[customHead]);
        fputs(buf1, ptr);
        customHead++;
    }
    fputs("end\n", ptr);
    fclose(ptr);
}

void runMachineWithOptionalPrint(turing* turingMachine, printParameters* printParams) {
    char bufmake[0x200];
    snprintf(bufmake, sizeof(bufmake), "./%s", printParams->OutputDir);
    mkdir(bufmake, 0700);
    char bufmake1[0x200];
    snprintf(bufmake1, sizeof(bufmake1), "./%s/Input/", printParams->OutputDir);
    mkdir(bufmake1, 0700);
    writeMachineToFile(turingMachine, printParams);
    while (!turingMachine->halted) {
        oneStep(turingMachine);
        if (turingMachine->currentState == printParams->printState) {
            writeMachineToFile(turingMachine, printParams);
        }
    }
    char command0[400];
    snprintf(command0, sizeof(command0), "cp ./Image-Converter.py ./%s/Image-Converter.py", printParams->OutputDir);
    system(command0);
    char command[400];
    snprintf(command, sizeof(command), "python3 ./%s/Image-Converter.py ./%s/Input", printParams->OutputDir, printParams->OutputDir);
    system(command);
}

void runMachineWithOptionalPrintSlowy(turing* turingMachine, printParameters* printParams) {
    char bufmake[0x200];
    snprintf(bufmake, sizeof(bufmake), "./%s", printParams->OutputDir);
    mkdir(bufmake, 0700);
    char bufmake1[0x200];
    snprintf(bufmake1, sizeof(bufmake1), "./%s/Input/", printParams->OutputDir);
    mkdir(bufmake1, 0700);
    writeMachineToFile(turingMachine, printParams);
    while (!turingMachine->halted) {
        oneStep(turingMachine);
        if (turingMachine->currentState == printParams->printState) {
            writeMachineToFile(turingMachine, printParams);
        }
    }
    char command0[400];
    snprintf(command0, sizeof(command0), "cp ./Image-ConverterSlowy.py ./%s/Image-ConverterSlowy.py", printParams->OutputDir);
    system(command0);
    char command[400];
    snprintf(command, sizeof(command), "python3 ./%s/Image-ConverterSlowy.py ./%s/Input", printParams->OutputDir, printParams->OutputDir);
    system(command);
}



void runMachineWithOptionalPrint2D(turing* turingMachine, printParameters* printParams, int howManyPrintsMax) {
    //le char 0 est le blank
    //le char 1 et 2 sont les print
    //le char 3 est pour la barrière du centre
    //les char du centre sont 4,5,6 pour vide right et down
    //le char right est 7
    //le char down est 8
    char bufmake[0x200];
    snprintf(bufmake, sizeof(bufmake), "./%s", printParams->OutputDir);
    mkdir(bufmake, 0700);
    char bufmake1[0x200];
    snprintf(bufmake1, sizeof(bufmake1), "./%s/Input/", printParams->OutputDir);
    mkdir(bufmake1, 0700);
    writeMachineToFile(turingMachine, printParams);
    int prints = 1;
    while (!turingMachine->halted && prints < howManyPrintsMax) {
        oneStep(turingMachine);
        if (turingMachine->currentState == printParams->printState) {
            writeMachineToFile(turingMachine, printParams);
            prints++;
            //optional
            printMachine(turingMachine);
        }
    }
    char command0[400];
    snprintf(command0, sizeof(command0), "cp ./Image-Converter-2D.py ./%s/Image-Converter-2D.py", printParams->OutputDir);
    system(command0);
    char command[400];
    snprintf(command, sizeof(command), "python3 ./%s/Image-Converter-2D.py ./%s/Input", printParams->OutputDir, printParams->OutputDir);
    system(command);
}


int makeASpeedyCar(int* alphabetSize, int blank, int* stateSize, int emptyHandState, int carSpeed, int* transitionCount, tempTransitions* transitions, int goingLeftState, int rightPrintChar, int haltState) {
    // 2 solutions, retirer n voitures puis faire apparaitre n voitures, ou retirer une voiture 1 par 1
    //j'ai fait option 2
    
    // goingLeft
    transitions[*transitionCount].currentState = goingLeftState; transitions[*transitionCount].currentChar = *alphabetSize; transitions[*transitionCount].nextState = goingLeftState; transitions[*transitionCount].nextDirection = -1; transitions[(*transitionCount)++].nextChar = *alphabetSize;

    //pick up the tail
    transitions[*transitionCount].currentState = emptyHandState; transitions[*transitionCount].currentChar = *alphabetSize; transitions[*transitionCount].nextState = (*stateSize)++; transitions[*transitionCount].nextDirection = 1; transitions[(*transitionCount)++].nextChar = blank;  
    //for every amount of back and forths left to do
    for (int i = 0; i < carSpeed-1; i++) {
        //propagate forward
        transitions[*transitionCount].currentState = (*stateSize)-1; transitions[*transitionCount].currentChar = *alphabetSize; transitions[*transitionCount].nextState = (*stateSize)-1; transitions[*transitionCount].nextDirection = 1; transitions[(*transitionCount)++].nextChar = *alphabetSize;
        
        //if printChar end simulation
        transitions[*transitionCount].currentState = (*stateSize)-1; transitions[*transitionCount].currentChar = rightPrintChar; transitions[*transitionCount].nextState = haltState; transitions[*transitionCount].nextDirection = 0; transitions[(*transitionCount)++].nextChar = rightPrintChar;
        
        //place
        transitions[*transitionCount].currentState = (*stateSize)-1; transitions[*transitionCount].currentChar = blank; transitions[*transitionCount].nextState = (*stateSize)++; transitions[*transitionCount].nextDirection = -1; transitions[(*transitionCount)++].nextChar = *alphabetSize;
        //propagate backwards
        transitions[*transitionCount].currentState = (*stateSize)-1; transitions[*transitionCount].currentChar = *alphabetSize; transitions[*transitionCount].nextState = (*stateSize)-1; transitions[*transitionCount].nextDirection = -1; transitions[(*transitionCount)++].nextChar = *alphabetSize;
        //go forward to tail
        transitions[*transitionCount].currentState = (*stateSize)-1; transitions[*transitionCount].currentChar = blank; transitions[*transitionCount].nextState = (*stateSize)++; transitions[*transitionCount].nextDirection = 1; transitions[(*transitionCount)++].nextChar = blank;
        //pick up car then loop
        transitions[*transitionCount].currentState = (*stateSize)-1; transitions[*transitionCount].currentChar = *alphabetSize; transitions[*transitionCount].nextState = (*stateSize)++; transitions[*transitionCount].nextDirection = 1; transitions[(*transitionCount)++].nextChar = blank;
    }
    //propagate a final time
    transitions[*transitionCount].currentState = (*stateSize)-1; transitions[*transitionCount].currentChar = *alphabetSize; transitions[*transitionCount].nextState = (*stateSize)-1; transitions[*transitionCount].nextDirection = 1; transitions[(*transitionCount)++].nextChar = *alphabetSize;
    //place and start going forwards again
    transitions[*transitionCount].currentState = (*stateSize)-1; transitions[*transitionCount].currentChar = blank; transitions[*transitionCount].nextState = emptyHandState; transitions[*transitionCount].nextDirection = 1; transitions[(*transitionCount)++].nextChar = *alphabetSize;
    return((*alphabetSize)++);
}

void placeASpeedyCar(int carSize, int carStartIndex, int carSymbol, int* tape) {
    for (int i = carStartIndex; i < carStartIndex + carSize; i++) {
        tape[i] = carSymbol;
    }
}

void addASpeedyCar(int* alphabetSize, int blank, int* stateSize, int emptyHandState, int carSpeed, int* transitionCount, tempTransitions* transitions, int goingLeftState, int rightPrintChar, int haltState, int carSize, int carStartIndex, int* tape) {
    placeASpeedyCar(carSize, carStartIndex, makeASpeedyCar(alphabetSize, blank, stateSize, emptyHandState, carSpeed, transitionCount, transitions, goingLeftState, rightPrintChar, haltState), tape);
}




//si jamais une voiture est à moins de deux fois la distance qu'on va avancer d'une autre voiture, prendre la vitesse de la voiture devant
int instantCrashAvoid(int* alphabetSize, int blank, int* stateSize, int emptyHandState, int offset, int maxSpeed, int* transitionCount, tempTransitions* transitions, int goingLeftState, int rightPrintChar, int haltState) {
    (*alphabetSize) += maxSpeed;
    //state i*2+offset = contamination associé à vi cycle sur 0
    for(int i = 1; i<= maxSpeed; i++) {
        //printf("%d\n", *transitionCount);
        //left until car
        transitions[*transitionCount].currentState = i*2+offset; transitions[*transitionCount].currentChar = blank; transitions[*transitionCount].nextState = i*2+offset; transitions[*transitionCount].nextDirection = -1; transitions[(*transitionCount)++].nextChar = blank;
        for (int j = i+1; j<= maxSpeed; j++){//voiture de derrière plus vite
        //if car start contaminating
        transitions[*transitionCount].currentState = i*2+offset; transitions[*transitionCount].currentChar = j; transitions[*transitionCount].nextState = i*2+1+offset; transitions[*transitionCount].nextDirection = -1; transitions[(*transitionCount)++].nextChar = i;
        //continue contaminating
        transitions[*transitionCount].currentState = i*2+1+offset; transitions[*transitionCount].currentChar = j; transitions[*transitionCount].nextState = i*2+1+offset; transitions[*transitionCount].nextDirection = -1; transitions[(*transitionCount)++].nextChar = i;
        //start going Right again
        //printf("%d lol\n", emptyHandState);
        transitions[*transitionCount].currentState = i*2+1+offset; transitions[*transitionCount].currentChar = blank; transitions[*transitionCount].nextState = emptyHandState; transitions[*transitionCount].nextDirection = 1; transitions[(*transitionCount)++].nextChar = blank;
        }

    }
    (*stateSize) += 2*maxSpeed+1;

    //printf("%d XD\n", *stateSize);
    for(int i = 1; i<= maxSpeed; i++) {
        //printf("%d\n", *transitionCount);
        //start going to head
        transitions[*transitionCount].currentState = emptyHandState; transitions[*transitionCount].currentChar = i; transitions[*transitionCount].nextState = (*stateSize)++; transitions[*transitionCount].nextDirection = 1; transitions[(*transitionCount)++].nextChar = i;
        //keep going to head
        transitions[*transitionCount].currentState = *stateSize - 1; transitions[*transitionCount].currentChar = i; transitions[*transitionCount].nextState = *stateSize - 1; transitions[*transitionCount].nextDirection = 1; transitions[(*transitionCount)++].nextChar = i;
        //if at end

        //printf("%d\n", *stateSize);
        //printf("%d\n", i);
        for (int j = 0; j<i; j++) {
            transitions[*transitionCount].currentState = *stateSize - 1; transitions[*transitionCount].currentChar = blank; transitions[*transitionCount].nextState = (*stateSize)++; transitions[*transitionCount].nextDirection = 1; transitions[(*transitionCount)++].nextChar = blank;
            for (int k = 1; k<i; k++){
               transitions[*transitionCount].currentState = *stateSize - 1; transitions[*transitionCount].currentChar = k; transitions[*transitionCount].nextState = k*2+offset; transitions[*transitionCount].nextDirection = -1; transitions[(*transitionCount)++].nextChar = k; 
            }
        }
        transitions[*transitionCount].currentState = *stateSize - 1; transitions[*transitionCount].currentChar = blank; transitions[*transitionCount].nextState = (*stateSize)++; transitions[*transitionCount].nextDirection = -1; transitions[(*transitionCount)++].nextChar = blank;
        transitions[*transitionCount].currentState = *stateSize - 1; transitions[*transitionCount].currentChar = blank; transitions[*transitionCount].nextState = *stateSize - 1; transitions[*transitionCount].nextDirection = -1; transitions[(*transitionCount)++].nextChar = blank;
        //when see car
        transitions[*transitionCount].currentState = *stateSize - 1; transitions[*transitionCount].currentChar = i; transitions[*transitionCount].nextState = (*stateSize)++; transitions[*transitionCount].nextDirection = -1; transitions[(*transitionCount)++].nextChar = i;
        transitions[*transitionCount].currentState = *stateSize - 1; transitions[*transitionCount].currentChar = i; transitions[*transitionCount].nextState = (*stateSize) - 1; transitions[*transitionCount].nextDirection = -1; transitions[(*transitionCount)++].nextChar = i;
        //when see empty
        transitions[*transitionCount].currentState = *stateSize - 1; transitions[*transitionCount].currentChar = blank; transitions[*transitionCount].nextState = (*stateSize)++; transitions[*transitionCount].nextDirection = 1; transitions[(*transitionCount)++].nextChar = blank;
        
        // goingLeft
        transitions[*transitionCount].currentState = goingLeftState; transitions[*transitionCount].currentChar = i; transitions[*transitionCount].nextState = goingLeftState; transitions[*transitionCount].nextDirection = -1; transitions[(*transitionCount)++].nextChar = i;

        //pick up the tail
        transitions[*transitionCount].currentState = *stateSize - 1; transitions[*transitionCount].currentChar = i; transitions[*transitionCount].nextState = (*stateSize)++; transitions[*transitionCount].nextDirection = 1; transitions[(*transitionCount)++].nextChar = blank;  
        //for every amount of back and forths left to do
        for (int j = 0; j < i-1; j++) {
            //propagate forward
            transitions[*transitionCount].currentState = (*stateSize)-1; transitions[*transitionCount].currentChar = i; transitions[*transitionCount].nextState = (*stateSize)-1; transitions[*transitionCount].nextDirection = 1; transitions[(*transitionCount)++].nextChar = i;
            
            //if printChar end simulation
            transitions[*transitionCount].currentState = (*stateSize)-1; transitions[*transitionCount].currentChar = rightPrintChar; transitions[*transitionCount].nextState = haltState; transitions[*transitionCount].nextDirection = 0; transitions[(*transitionCount)++].nextChar = rightPrintChar;
            
            //place
            transitions[*transitionCount].currentState = (*stateSize)-1; transitions[*transitionCount].currentChar = blank; transitions[*transitionCount].nextState = (*stateSize)++; transitions[*transitionCount].nextDirection = -1; transitions[(*transitionCount)++].nextChar = i;
            //propagate backwards
            transitions[*transitionCount].currentState = (*stateSize)-1; transitions[*transitionCount].currentChar = i; transitions[*transitionCount].nextState = (*stateSize)-1; transitions[*transitionCount].nextDirection = -1; transitions[(*transitionCount)++].nextChar = i;
            //go forward to tail
            transitions[*transitionCount].currentState = (*stateSize)-1; transitions[*transitionCount].currentChar = blank; transitions[*transitionCount].nextState = (*stateSize)++; transitions[*transitionCount].nextDirection = 1; transitions[(*transitionCount)++].nextChar = blank;
            //pick up car then loop
            transitions[*transitionCount].currentState = (*stateSize)-1; transitions[*transitionCount].currentChar = i; transitions[*transitionCount].nextState = (*stateSize)++; transitions[*transitionCount].nextDirection = 1; transitions[(*transitionCount)++].nextChar = blank;
        }
        //propagate a final time
        transitions[*transitionCount].currentState = (*stateSize)-1; transitions[*transitionCount].currentChar = i; transitions[*transitionCount].nextState = (*stateSize)-1; transitions[*transitionCount].nextDirection = 1; transitions[(*transitionCount)++].nextChar = i;
        //place and start going forwards again
        transitions[*transitionCount].currentState = (*stateSize)-1; transitions[*transitionCount].currentChar = blank; transitions[*transitionCount].nextState = emptyHandState; transitions[*transitionCount].nextDirection = 1; transitions[(*transitionCount)++].nextChar = i;
        
        
    }
    return(*alphabetSize);
}





int main()
{


//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// 3 STATE BUSY BEAVER TEST

/*

int BB3alphabetSize = 2;
int BB3blank = 0;
int BB3stateSize = 4; // includes HALT
int BB3starterState = 0;
int BB3finalStates[1] = {3};
int BB3finalStatesSize = 1;
tempTransitions* BB3transitions = malloc(sizeof(tempTransitions)*6);
BB3transitions[0].currentState = 0; BB3transitions[0].currentChar = 0; BB3transitions[0].nextState = 1; BB3transitions[0].nextDirection = 1; BB3transitions[0].nextChar = 1;
BB3transitions[1].currentState = 0; BB3transitions[1].currentChar = 1; BB3transitions[1].nextState = 2; BB3transitions[1].nextDirection = -1; BB3transitions[1].nextChar = 1;
BB3transitions[2].currentState = 1; BB3transitions[2].currentChar = 0; BB3transitions[2].nextState = 0; BB3transitions[2].nextDirection = -1; BB3transitions[2].nextChar = 1;
BB3transitions[3].currentState = 1; BB3transitions[3].currentChar = 1; BB3transitions[3].nextState = 1; BB3transitions[3].nextDirection = 1; BB3transitions[3].nextChar = 1;
BB3transitions[4].currentState = 2; BB3transitions[4].currentChar = 0; BB3transitions[4].nextState = 1; BB3transitions[4].nextDirection = -1; BB3transitions[4].nextChar = 1;
BB3transitions[5].currentState = 2; BB3transitions[5].currentChar = 1; BB3transitions[5].nextState = 3; BB3transitions[5].nextDirection = 1; BB3transitions[5].nextChar = 1;
int BB3transitionsSize = 6;
int BB3startingTape[1] = {0};
int BB3startingTapeLength = 1;
int BB3headPosition = 0;

turing* BB3machine = makeTuringMachine(BB3alphabetSize, BB3blank, BB3stateSize, BB3starterState, BB3finalStates, BB3finalStatesSize, BB3transitions, BB3transitionsSize, BB3startingTape, BB3startingTapeLength, BB3headPosition);
runMachine(BB3machine);

*/


//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// PROBLÈME 1
/*
int PB1alphabetSize = 3;
int PB1blank = 0;
int PB1stateSize = 3; // includes HALT
int PB1starterState = 0;
int PB1finalStates[1] = {2};
int PB1finalStatesSize = 1; 
tempTransitions* PB1transitions = malloc(sizeof(tempTransitions)*6);
PB1transitions[0].currentState = 0; PB1transitions[0].currentChar = 0; PB1transitions[0].nextState = 0; PB1transitions[0].nextDirection = 1; PB1transitions[0].nextChar = 0;
PB1transitions[1].currentState = 0; PB1transitions[1].currentChar = 1; PB1transitions[1].nextState = 1; PB1transitions[1].nextDirection = 1; PB1transitions[1].nextChar = 0;
PB1transitions[2].currentState = 0; PB1transitions[2].currentChar = 2; PB1transitions[2].nextState = 2; PB1transitions[2].nextDirection = 0; PB1transitions[2].nextChar = 2;
PB1transitions[3].currentState = 1; PB1transitions[3].currentChar = 0; PB1transitions[3].nextState = 0; PB1transitions[3].nextDirection = 1; PB1transitions[3].nextChar = 1;
PB1transitions[4].currentState = 1; PB1transitions[4].currentChar = 1; PB1transitions[4].nextState = 1; PB1transitions[4].nextDirection = 1; PB1transitions[4].nextChar = 1;
PB1transitions[5].currentState = 1; PB1transitions[5].currentChar = 2; PB1transitions[5].nextState = 2; PB1transitions[5].nextDirection = 0; PB1transitions[5].nextChar = 2;
int PB1transitionsSize = 6;
// en fonction des tests qu'on veut
int PB1startingTape[30] = {0,1,0,1,1,1,1,1,0,0,0,1,1,1,0,0,0,0,1,1,0,0,1,0,1,0,1,0,0,2};
int PB1startingTapeLength = 30;
int PB1headPosition = 0;

turing* PB1machine = makeTuringMachine(PB1alphabetSize, PB1blank, PB1stateSize, PB1starterState, PB1finalStates, PB1finalStatesSize, PB1transitions, PB1transitionsSize, PB1startingTape, PB1startingTapeLength, PB1headPosition);
runMachine(PB1machine);
*/


//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//FANCY PRINT TEST
/*
int FPTestalphabetSize = 4; //0, 1 | print : 2 , 3
int FPTestblank = 0;
int FPTeststateSize = 5; //handEmpty, handFull, goingBackLeft, print, HALT
int FPTeststarterState = 0;
int FPTestfinalStates[1] = {4};
int FPTestfinalStatesSize = 1; 
tempTransitions* FPTesttransitions = malloc(sizeof(tempTransitions)*10);
FPTesttransitions[0].currentState = 0; FPTesttransitions[0].currentChar = 0; FPTesttransitions[0].nextState = 0; FPTesttransitions[0].nextDirection = 1; FPTesttransitions[0].nextChar = 0;
FPTesttransitions[1].currentState = 0; FPTesttransitions[1].currentChar = 1; FPTesttransitions[1].nextState = 1; FPTesttransitions[1].nextDirection = 1; FPTesttransitions[1].nextChar = 0;
FPTesttransitions[2].currentState = 0; FPTesttransitions[2].currentChar = 3; FPTesttransitions[2].nextState = 3; FPTesttransitions[2].nextDirection = 0; FPTesttransitions[2].nextChar = 3; //if on rightStop and not finished (handEmpty) print
FPTesttransitions[3].currentState = 3; FPTesttransitions[3].currentChar = 3; FPTesttransitions[3].nextState = 2; FPTesttransitions[3].nextDirection = -1; FPTesttransitions[3].nextChar = 3; //begin GoingLeft
FPTesttransitions[4].currentState = 2; FPTesttransitions[4].currentChar = 0; FPTesttransitions[4].nextState = 2; FPTesttransitions[4].nextDirection = -1; FPTesttransitions[4].nextChar = 0; //read0 GoingLeft
FPTesttransitions[5].currentState = 2; FPTesttransitions[5].currentChar = 1; FPTesttransitions[5].nextState = 2; FPTesttransitions[5].nextDirection = -1; FPTesttransitions[5].nextChar = 1; //read1 GoingLeft
FPTesttransitions[6].currentState = 2; FPTesttransitions[6].currentChar = 2; FPTesttransitions[6].nextState = 0; FPTesttransitions[6].nextDirection = 1; FPTesttransitions[6].nextChar = 2; //read leftStop GoingLeft
FPTesttransitions[7].currentState = 1; FPTesttransitions[7].currentChar = 0; FPTesttransitions[7].nextState = 0; FPTesttransitions[7].nextDirection = 1; FPTesttransitions[7].nextChar = 1;
FPTesttransitions[8].currentState = 1; FPTesttransitions[8].currentChar = 1; FPTesttransitions[8].nextState = 1; FPTesttransitions[8].nextDirection = 1; FPTesttransitions[8].nextChar = 1;
FPTesttransitions[9].currentState = 1; FPTesttransitions[9].currentChar = 3; FPTesttransitions[9].nextState = 4; FPTesttransitions[9].nextDirection = 0; FPTesttransitions[9].nextChar = 3; //if on rightStop and finished (handFull) HALT

int FPTesttransitionsSize = 10;
int* FPTeststartingTape = malloc(sizeof(int)*200);
int FPTeststartingTapeLength = 200;
FPTeststartingTape[0] = 2;
FPTeststartingTape[199] = 3;
for (int i = 1; i<50; i++) {
    FPTeststartingTape[i] = 0;
}
for (int i = 50; i<150; i++) {
    FPTeststartingTape[i] = 1;
}
for (int i = 150; i<199; i++) {
    FPTeststartingTape[i] = 0;
}
int FPTestheadPosition = 1;

turing* FPTestmachine = makeTuringMachine(FPTestalphabetSize, FPTestblank, FPTeststateSize, FPTeststarterState, FPTestfinalStates, FPTestfinalStatesSize, FPTesttransitions, FPTesttransitionsSize, FPTeststartingTape, FPTeststartingTapeLength, FPTestheadPosition);

// Run FPTest
//runMachine(FPTestmachine);



printParameters* FPTestprintParams = malloc(sizeof(printParameters));
int FPTestfileCount = 10000;
FPTestprintParams->fileCount = &FPTestfileCount;
FPTestprintParams->OutputDir = "FPTest";
FPTestprintParams->leftBorderChar = 2;
FPTestprintParams->rightBorderChar = 3;
FPTestprintParams->printState = 3;



runMachineWithOptionalPrint(FPTestmachine, FPTestprintParams);
//writeMachineToFile(FPTestmachine, FPTestprintParams);   

printf("finished\n");

*/


//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// PROBLÈME 2
///*
int speedyCarsTestalphabetSize = 3; //0 | print : 1 , 2
int speedyCarsTestblank = 0;
int speedyCarsTeststateSize = 4; //handEmpty, goingBackLeft, print, HALT
int speedyCarsTeststarterState = 0;
int speedyCarsTestfinalStates[1] = {3};
int speedyCarsTestfinalStatesSize = 1; 
tempTransitions* speedyCarsTesttransitions = malloc(sizeof(tempTransitions)*1000);
speedyCarsTesttransitions[0].currentState = 0; speedyCarsTesttransitions[0].currentChar = 0; speedyCarsTesttransitions[0].nextState = 0; speedyCarsTesttransitions[0].nextDirection = 1; speedyCarsTesttransitions[0].nextChar = 0; // transit right
speedyCarsTesttransitions[1].currentState = 0; speedyCarsTesttransitions[1].currentChar = 2; speedyCarsTesttransitions[1].nextState = 2; speedyCarsTesttransitions[1].nextDirection = 0; speedyCarsTesttransitions[1].nextChar = 2; //if on rightStop and not finished (handEmpty) print
speedyCarsTesttransitions[2].currentState = 2; speedyCarsTesttransitions[2].currentChar = 2; speedyCarsTesttransitions[2].nextState = 1; speedyCarsTesttransitions[2].nextDirection = -1; speedyCarsTesttransitions[2].nextChar = 2; //begin GoingLeft
speedyCarsTesttransitions[3].currentState = 1; speedyCarsTesttransitions[3].currentChar = 0; speedyCarsTesttransitions[3].nextState = 1; speedyCarsTesttransitions[3].nextDirection = -1; speedyCarsTesttransitions[3].nextChar = 0; //read0 GoingLeft
speedyCarsTesttransitions[4].currentState = 1; speedyCarsTesttransitions[4].currentChar = 1; speedyCarsTesttransitions[4].nextState = 0; speedyCarsTesttransitions[4].nextDirection = 1; speedyCarsTesttransitions[4].nextChar = 1; //read leftStop GoingLeft

int speedyCarsTesttransitionsSize = 5;
int* speedyCarsTeststartingTape = malloc(sizeof(int)*3000);
int speedyCarsTeststartingTapeLength = 3000;
speedyCarsTeststartingTape[0] = 1;
for (int i = 1; i<3000; i++) {
    speedyCarsTeststartingTape[i] = 0;
}
for (int i = 2980; i<3000; i++) {
    speedyCarsTeststartingTape[i] = 2;
}


int speedyCarsTestheadPosition = 1;

printParameters* speedyCarsTestprintParams = malloc(sizeof(printParameters));
int speedyCarsTestfileCount = 10000;
speedyCarsTestprintParams->fileCount = &speedyCarsTestfileCount;
speedyCarsTestprintParams->leftBorderChar = 1;
speedyCarsTestprintParams->rightBorderChar = 2;
speedyCarsTestprintParams->printState = 2;




//two 100 length cars, starting 100 apart; speeds : first = 5 second = 3
/*
speedyCarsTestprintParams->OutputDir = "twoSpeedyCars";
addASpeedyCar(&speedyCarsTestalphabetSize, speedyCarsTestblank, &speedyCarsTeststateSize, 0, 5, &speedyCarsTesttransitionsSize, speedyCarsTesttransitions, 1, 2, 3, 100, 50, speedyCarsTeststartingTape);
addASpeedyCar(&speedyCarsTestalphabetSize, speedyCarsTestblank, &speedyCarsTeststateSize, 0, 3, &speedyCarsTesttransitionsSize, speedyCarsTesttransitions, 1, 2, 3, 100, 250, speedyCarsTeststartingTape);
*/

//three 100 length cars, starting 200 apart; speeds : first = 12 second = 10 third = 8

speedyCarsTestprintParams->OutputDir = "threeSpeedyCarsCrashing";
addASpeedyCar(&speedyCarsTestalphabetSize, speedyCarsTestblank, &speedyCarsTeststateSize, 0, 12, &speedyCarsTesttransitionsSize, speedyCarsTesttransitions, 1, 2, 3, 100, 50, speedyCarsTeststartingTape);
addASpeedyCar(&speedyCarsTestalphabetSize, speedyCarsTestblank, &speedyCarsTeststateSize, 0, 10, &speedyCarsTesttransitionsSize, speedyCarsTesttransitions, 1, 2, 3, 100, 350, speedyCarsTeststartingTape);
addASpeedyCar(&speedyCarsTestalphabetSize, speedyCarsTestblank, &speedyCarsTeststateSize, 0, 8, &speedyCarsTesttransitionsSize, speedyCarsTesttransitions, 1, 2, 3, 100, 650, speedyCarsTeststartingTape);



//three 100 length cars, starting 100 apart; speeds : first = 5 second = 10 third = 20
/*
speedyCarsTestprintParams->OutputDir = "threeSpeedyCarsDispersing";
addASpeedyCar(&speedyCarsTestalphabetSize, speedyCarsTestblank, &speedyCarsTeststateSize, 0, 7, &speedyCarsTesttransitionsSize, speedyCarsTesttransitions, 1, 2, 3, 100, 50, speedyCarsTeststartingTape);
addASpeedyCar(&speedyCarsTestalphabetSize, speedyCarsTestblank, &speedyCarsTeststateSize, 0, 10, &speedyCarsTesttransitionsSize, speedyCarsTesttransitions, 1, 2, 3, 100, 250, speedyCarsTeststartingTape);
addASpeedyCar(&speedyCarsTestalphabetSize, speedyCarsTestblank, &speedyCarsTeststateSize, 0, 20, &speedyCarsTesttransitionsSize, speedyCarsTesttransitions, 1, 2, 3, 100, 450, speedyCarsTeststartingTape);
*/

turing* speedyCarsTestmachine = makeTuringMachine(speedyCarsTestalphabetSize, speedyCarsTestblank, speedyCarsTeststateSize, speedyCarsTeststarterState, speedyCarsTestfinalStates, speedyCarsTestfinalStatesSize, speedyCarsTesttransitions, speedyCarsTesttransitionsSize, speedyCarsTeststartingTape, speedyCarsTeststartingTapeLength, speedyCarsTestheadPosition);




//runMachine(speedyCarsTestmachine);


runMachineWithOptionalPrint(speedyCarsTestmachine, speedyCarsTestprintParams);

//*/


//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// PROBLÈME 3
///*
int slowyCarsTestalphabetSize = 3; //0 | print : 1 , 2
int slowyCarsTestblank = 0;
int slowyCarsTeststateSize = 4; //handEmpty, goingBackLeft, print, HALT
int slowyCarsTeststarterState = 0;
int slowyCarsTestfinalStates[1] = {3};
int slowyCarsTestfinalStatesSize = 1; 
tempTransitions* slowyCarsTesttransitions = malloc(sizeof(tempTransitions)*100000);
slowyCarsTesttransitions[0].currentState = 0; slowyCarsTesttransitions[0].currentChar = 0; slowyCarsTesttransitions[0].nextState = 0; slowyCarsTesttransitions[0].nextDirection = 1; slowyCarsTesttransitions[0].nextChar = 0; // transit right
slowyCarsTesttransitions[1].currentState = 0; slowyCarsTesttransitions[1].currentChar = -2; slowyCarsTesttransitions[1].nextState = 2; slowyCarsTesttransitions[1].nextDirection = 0; slowyCarsTesttransitions[1].nextChar = -2; //if on rightStop and not finished (handEmpty) print
slowyCarsTesttransitions[2].currentState = 2; slowyCarsTesttransitions[2].currentChar = -2; slowyCarsTesttransitions[2].nextState = 1; slowyCarsTesttransitions[2].nextDirection = -1; slowyCarsTesttransitions[2].nextChar = -2; //begin GoingLeft
slowyCarsTesttransitions[3].currentState = 1; slowyCarsTesttransitions[3].currentChar = 0; slowyCarsTesttransitions[3].nextState = 1; slowyCarsTesttransitions[3].nextDirection = -1; slowyCarsTesttransitions[3].nextChar = 0; //read0 GoingLeft
slowyCarsTesttransitions[4].currentState = 1; slowyCarsTesttransitions[4].currentChar = -1; slowyCarsTesttransitions[4].nextState = 0; slowyCarsTesttransitions[4].nextDirection = 1; slowyCarsTesttransitions[4].nextChar = -1; //read leftStop GoingLeft

int slowyCarsTesttransitionsSize = 5;
int* slowyCarsTeststartingTape = malloc(sizeof(int)*3000);

int slowyCarsTeststartingTapeLength = 3000;

int slowyCarsTestheadPosition = 1;

slowyCarsTeststartingTape[0] = -1;


for (int i = 1; i<3000; i++) {
    slowyCarsTeststartingTape[i] = 0;
}



for (int i = 2980; i<3000; i++) {
    slowyCarsTeststartingTape[i] = -2;
}



printParameters* slowyCarsTestprintParams = malloc(sizeof(printParameters));
int slowyCarsTestfileCount = 10000;
slowyCarsTestprintParams->fileCount = &slowyCarsTestfileCount;
slowyCarsTestprintParams->leftBorderChar = -1;
slowyCarsTestprintParams->rightBorderChar = -2;
slowyCarsTestprintParams->printState = 2;


slowyCarsTestprintParams->OutputDir = "threeSpeedyCarsSlowing";

instantCrashAvoid(&slowyCarsTestalphabetSize, slowyCarsTestblank, &slowyCarsTeststateSize, 0, slowyCarsTesttransitionsSize, 12, &slowyCarsTesttransitionsSize, slowyCarsTesttransitions, 1, -2, 3);

placeASpeedyCar(100, 100, 12, slowyCarsTeststartingTape);
placeASpeedyCar(100, 350, 10, slowyCarsTeststartingTape);
placeASpeedyCar(100, 550, 8, slowyCarsTeststartingTape);

turing* slowyCarsTestmachine = makeTuringMachine(slowyCarsTestalphabetSize*30, slowyCarsTestblank, slowyCarsTeststateSize*30, slowyCarsTeststarterState, slowyCarsTestfinalStates, slowyCarsTestfinalStatesSize, slowyCarsTesttransitions, slowyCarsTesttransitionsSize, slowyCarsTeststartingTape, slowyCarsTeststartingTapeLength, slowyCarsTestheadPosition);

//runMachine(slowyCarsTestmachine);


//runMachineWithOptionalPrintSlowy(slowyCarsTestmachine, slowyCarsTestprintParams);



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//PROBLEM 4

int PB4alphabetSize = 9; // 0 = empty, 1 = borderLeft, 2 = borderRight, 3 = borderCenter, 4 = centerEmpty, 5 = centerRight, 6 = centerDown, 7 = Right, 8 = Down
int PB4blank = 0;
int PB4stateSize = 23; // 0 = phase1.handEmpty, 1 = phase1.handFull, 2 = phase3.handEmpty, 3 = phase2.libérerCentre, 4 = phase2.revenir-handfull, 5 = phase3.handFull, 6 = phase2.placercentre, 7 = phase1.retourenarrière, 8 = phase1.repartirenavant, 9 = phase2.revenir-handEmpty, 
                      // 10 = phase5.handEmpty, 11 = phase5.handFull, 12 = phase7.handEmpty, 13 = phase6.libérerCentre, 14 = phase6.revenir-handfull, 15 = phase7.handFull, 16 = phase6.placercentre, 17 = phase5.retourenarrière, 18 = phase5.repartirenavant, 19 = phase6.revenir-handEmpty
                      // 20 = phase8.retour
                      // 21 = phase8.PrintState
                      // 22 = HaltState (not used)

//phase 1 : 0 = phase1.handEmpty, 1 = phase1.handFull, 7 = phase1.retourenarrière, 8 = phase1.repartirenavant
//phase 2 : 3 = phase2.libérerCentre, 4 = phase2.revenir-handfull, 6 = phase2.placercentre, 9 = phase2.revenir-handEmpty
//phase 3 : 2 = phase3.handEmpty, 5 = phase3.handFull
//phase 4 : instantané
//phase 5 : 10 = phase5.handEmpty, 11 = phase5.handFull, 17 = phase5.retourenarrière, 18 = phase5.repartirenavant
//phase 6 : 13 = phase6.libérerCentre, 14 = phase6.revenir-handfull, 16 = phase6.placercentre, 19 = phase6.revenir-handEmpty
//phase 7 : 12 = phase7.handEmpty, 15 = phase7.handFull
//phase 8 : 20 = phase8.retour, 21 = phase8.PrintState

int PB4starterState = 0;
int PB4finalStates[1] = {22}; // NEED TO DO IT
int PB4finalStatesSize = 1; 
tempTransitions* PB4transitions = malloc(sizeof(tempTransitions)*89);
//phase 1 déplacer vers la droite
//phase 2 faire correspondre le centre si changement
//phase 3 finir le déplacement vers la droite
//phase 4 aller sur la bande verticale
//phase 5 déplacer vers le bas
//phase 6 faire correspondre le centre si changement
//phase 7 finir le déplacement vers le bas
//phase 8 revenir au début. Vinaver.exe

//PHASE 1 :
//si main vide
PB4transitions[0].currentState = 0; PB4transitions[0].currentChar = 0; PB4transitions[0].nextState = 0; PB4transitions[0].nextDirection = 1; PB4transitions[0].nextChar = 0;//pas de voiture en main ni sur tape alors avancer
PB4transitions[1].currentState = 0; PB4transitions[1].currentChar = 7; PB4transitions[1].nextState = 1; PB4transitions[1].nextDirection = 1; PB4transitions[1].nextChar = 0;//pas de voiture en main et sur tape alors retirer voiture
PB4transitions[2].currentState = 0; PB4transitions[2].currentChar = 6; PB4transitions[2].nextState = 2; PB4transitions[2].nextDirection = 1; PB4transitions[2].nextChar = 6;//si voiture descendante au centre ne rien faire passer en phase3 hand empty
PB4transitions[3].currentState = 0; PB4transitions[3].currentChar = 4; PB4transitions[3].nextState = 2; PB4transitions[3].nextDirection = 1; PB4transitions[3].nextChar = 4;//si rien au centre ne rien faire passer en phase3 avec empty hand
PB4transitions[4].currentState = 0; PB4transitions[4].currentChar = 5; PB4transitions[4].nextState = 3; PB4transitions[4].nextDirection = 1; PB4transitions[4].nextChar = 4;//si Right au centre vider et passer en phase2.vider

//si main pleine
PB4transitions[5].currentState = 1; PB4transitions[5].currentChar = 0; PB4transitions[5].nextState = 0; PB4transitions[5].nextDirection = 1; PB4transitions[5].nextChar = 7;//voiture en main mais pas sur tape alors placer et avancer
PB4transitions[6].currentState = 1; PB4transitions[6].currentChar = 7; PB4transitions[6].nextState = 1; PB4transitions[6].nextDirection = 1; PB4transitions[6].nextChar = 7;//voiture en main et sur tape alors avancer
PB4transitions[7].currentState = 1; PB4transitions[7].currentChar = 5; PB4transitions[7].nextState = 5; PB4transitions[7].nextDirection = 1; PB4transitions[7].nextChar = 5;//si Right au centre ne rien faire passer en phase3 hand full
PB4transitions[8].currentState = 1; PB4transitions[8].currentChar = 6; PB4transitions[8].nextState = 7; PB4transitions[8].nextDirection = -1; PB4transitions[8].nextChar = 6;//si Down au centre reposer la voiture en queue
PB4transitions[9].currentState = 1; PB4transitions[9].currentChar = 4; PB4transitions[9].nextState = 6; PB4transitions[9].nextDirection = 1; PB4transitions[9].nextChar = 5;//si rien au centre placer la voiture et passer en phase2.placer

//backtrace
PB4transitions[10].currentState = 7; PB4transitions[10].currentChar = 7; PB4transitions[10].nextState = 7; PB4transitions[10].nextDirection = -1; PB4transitions[10].nextChar = 7;//voiture en main et sur tape alors continuer à revenir
PB4transitions[11].currentState = 7; PB4transitions[11].currentChar = 0; PB4transitions[11].nextState = 8; PB4transitions[11].nextDirection = 1; PB4transitions[11].nextChar = 7;//voiture en main mais pas sur tape alors placer et repartir en avant
PB4transitions[12].currentState = 8; PB4transitions[12].currentChar = 7; PB4transitions[12].nextState = 8; PB4transitions[12].nextDirection = 1; PB4transitions[12].nextChar = 7;//si toujours pas au centre continuer à repartir
PB4transitions[13].currentState = 8; PB4transitions[13].currentChar = 6; PB4transitions[13].nextState = 2; PB4transitions[13].nextDirection = 1; PB4transitions[13].nextChar = 6;//lorsqu'au centre, avancer et se mettre en phase3.handEmpty


//PHASE 2 :
//si ordre de vider
    //si centre
PB4transitions[14].currentState = 3; PB4transitions[14].currentChar = 5; PB4transitions[14].nextState = 4; PB4transitions[14].nextDirection = -1; PB4transitions[14].nextChar = 4;//si centre vider et passer en phase3.retour-handFull
    //sinon
PB4transitions[15].currentState = 3; PB4transitions[15].currentChar = 0; PB4transitions[15].nextState = 3; PB4transitions[15].nextDirection = 1; PB4transitions[15].nextChar = 0;//continuer
PB4transitions[16].currentState = 3; PB4transitions[16].currentChar = 3; PB4transitions[16].nextState = 3; PB4transitions[16].nextDirection = 1; PB4transitions[16].nextChar = 3;//continuer
PB4transitions[17].currentState = 3; PB4transitions[17].currentChar = 7; PB4transitions[17].nextState = 3; PB4transitions[17].nextDirection = 1; PB4transitions[17].nextChar = 7;//continuer
PB4transitions[18].currentState = 3; PB4transitions[18].currentChar = 8; PB4transitions[18].nextState = 3; PB4transitions[18].nextDirection = 1; PB4transitions[18].nextChar = 8;//continuer
    //revenir
PB4transitions[19].currentState = 4; PB4transitions[19].currentChar = 0; PB4transitions[19].nextState = 4; PB4transitions[19].nextDirection = -1; PB4transitions[19].nextChar = 0;//revenir
PB4transitions[20].currentState = 4; PB4transitions[20].currentChar = 3; PB4transitions[20].nextState = 4; PB4transitions[20].nextDirection = -1; PB4transitions[20].nextChar = 3;//revenir
PB4transitions[21].currentState = 4; PB4transitions[21].currentChar = 7; PB4transitions[21].nextState = 4; PB4transitions[21].nextDirection = -1; PB4transitions[21].nextChar = 7;//revenir
PB4transitions[22].currentState = 4; PB4transitions[22].currentChar = 8; PB4transitions[22].nextState = 4; PB4transitions[22].nextDirection = -1; PB4transitions[22].nextChar = 8;//revenir
    //si centre se mettre en phase3.handFull
PB4transitions[23].currentState = 4; PB4transitions[23].currentChar = 4; PB4transitions[23].nextState = 5; PB4transitions[23].nextDirection = 1; PB4transitions[23].nextChar = 4;//passer en phase3 avec full hand

//si ordre de rajouter
    //si centre
PB4transitions[24].currentState = 6; PB4transitions[24].currentChar = 4; PB4transitions[24].nextState = 9; PB4transitions[24].nextDirection = -1; PB4transitions[24].nextChar = 5;//si centre placer et passer en phase3.retour-handEmpty
    //sinon
PB4transitions[25].currentState = 6; PB4transitions[25].currentChar = 0; PB4transitions[25].nextState = 6; PB4transitions[25].nextDirection = 1; PB4transitions[25].nextChar = 0;//continuer
PB4transitions[26].currentState = 6; PB4transitions[26].currentChar = 3; PB4transitions[26].nextState = 6; PB4transitions[26].nextDirection = 1; PB4transitions[26].nextChar = 3;//continuer
PB4transitions[27].currentState = 6; PB4transitions[27].currentChar = 7; PB4transitions[27].nextState = 6; PB4transitions[27].nextDirection = 1; PB4transitions[27].nextChar = 7;//continuer
PB4transitions[28].currentState = 6; PB4transitions[28].currentChar = 8; PB4transitions[28].nextState = 6; PB4transitions[28].nextDirection = 1; PB4transitions[28].nextChar = 8;//continuer
    //revenir
PB4transitions[29].currentState = 9; PB4transitions[29].currentChar = 0; PB4transitions[29].nextState = 9; PB4transitions[29].nextDirection = -1; PB4transitions[29].nextChar = 0;//revenir
PB4transitions[30].currentState = 9; PB4transitions[30].currentChar = 3; PB4transitions[30].nextState = 9; PB4transitions[30].nextDirection = -1; PB4transitions[30].nextChar = 3;//revenir
PB4transitions[31].currentState = 9; PB4transitions[31].currentChar = 7; PB4transitions[31].nextState = 9; PB4transitions[31].nextDirection = -1; PB4transitions[31].nextChar = 7;//revenir
PB4transitions[32].currentState = 9; PB4transitions[32].currentChar = 8; PB4transitions[32].nextState = 9; PB4transitions[32].nextDirection = -1; PB4transitions[32].nextChar = 8;//revenir
    //si centre se mettre en phase3.handFull
PB4transitions[33].currentState = 9; PB4transitions[33].currentChar = 5; PB4transitions[33].nextState = 2; PB4transitions[33].nextDirection = 1; PB4transitions[33].nextChar = 5;//passer en phase3 avec empty hand

//PHASE 3   
//HandEmpty
PB4transitions[34].currentState = 2; PB4transitions[34].currentChar = 0; PB4transitions[34].nextState = 2; PB4transitions[34].nextDirection = 1; PB4transitions[34].nextChar = 0;//pas de voiture en main ni sur tape alors avancer
PB4transitions[35].currentState = 2; PB4transitions[35].currentChar = 7; PB4transitions[35].nextState = 5; PB4transitions[35].nextDirection = 1; PB4transitions[35].nextChar = 0;//pas de voiture en main mais sur tape alors ramasser

//HandFull
PB4transitions[36].currentState = 5; PB4transitions[36].currentChar = 0; PB4transitions[36].nextState = 2; PB4transitions[36].nextDirection = 1; PB4transitions[36].nextChar = 7;//voiture en main mais sur tape alors placer
PB4transitions[37].currentState = 5; PB4transitions[37].currentChar = 7; PB4transitions[37].nextState = 5; PB4transitions[37].nextDirection = 1; PB4transitions[37].nextChar = 7;//voiture en main et sur tape alors avancer

//PHASE 4
//center border
PB4transitions[38].currentState = 2; PB4transitions[38].currentChar = 3; PB4transitions[38].nextState = 10; PB4transitions[38].nextDirection = 1; PB4transitions[38].nextChar = 3;//si border centre alors vider main et passer en phase 5 sur le bandeau vertical
PB4transitions[39].currentState = 5; PB4transitions[39].currentChar = 3; PB4transitions[39].nextState = 10; PB4transitions[39].nextDirection = 1; PB4transitions[39].nextChar = 3;//si border centre alors vider main et passer en phase 5 sur le bandeau vertical

//changements : inversion des caractères 7 et 8 et des caractères 5 et 6
//              copiage du centre se fait dans le sens inverse
//              décallage des états +10

//PHASE 5 :
//si main vide
PB4transitions[40].currentState = 10; PB4transitions[40].currentChar = 0; PB4transitions[40].nextState = 10; PB4transitions[40].nextDirection = 1; PB4transitions[40].nextChar = 0;//pas de voiture en main ni sur tape alors avancer
PB4transitions[41].currentState = 10; PB4transitions[41].currentChar = 8; PB4transitions[41].nextState = 11; PB4transitions[41].nextDirection = 1; PB4transitions[41].nextChar = 0;//pas de voiture en main et sur tape alors retirer voiture
PB4transitions[42].currentState = 10; PB4transitions[42].currentChar = 5; PB4transitions[42].nextState = 12; PB4transitions[42].nextDirection = 1; PB4transitions[42].nextChar = 5;//si Right au centre ne rien faire passer en phase7 hand empty
PB4transitions[43].currentState = 10; PB4transitions[43].currentChar = 4; PB4transitions[43].nextState = 12; PB4transitions[43].nextDirection = 1; PB4transitions[43].nextChar = 4;//si rien au centre ne rien faire passer en phase7 avec empty hand
PB4transitions[44].currentState = 10; PB4transitions[44].currentChar = 6; PB4transitions[44].nextState = 13; PB4transitions[44].nextDirection = -1; PB4transitions[44].nextChar = 4;//si Down au centre vider et passer en phase6.vider

//si main pleine
PB4transitions[45].currentState = 11; PB4transitions[45].currentChar = 0; PB4transitions[45].nextState = 10; PB4transitions[45].nextDirection = 1; PB4transitions[45].nextChar = 8;//voiture en main mais pas sur tape alors placer et avancer
PB4transitions[46].currentState = 11; PB4transitions[46].currentChar = 8; PB4transitions[46].nextState = 11; PB4transitions[46].nextDirection = 1; PB4transitions[46].nextChar = 8;//voiture en main et sur tape alors avancer
PB4transitions[47].currentState = 11; PB4transitions[47].currentChar = 6; PB4transitions[47].nextState = 15; PB4transitions[47].nextDirection = 1; PB4transitions[47].nextChar = 6;//si Down au centre ne rien faire passer en phase7 hand full
PB4transitions[48].currentState = 11; PB4transitions[48].currentChar = 5; PB4transitions[48].nextState = 17; PB4transitions[48].nextDirection = -1; PB4transitions[48].nextChar = 5;//si Right au centre reposer la voiture en queue
PB4transitions[49].currentState = 11; PB4transitions[49].currentChar = 4; PB4transitions[49].nextState = 16; PB4transitions[49].nextDirection = -1; PB4transitions[49].nextChar = 6;//si rien au centre placer la voiture et passer en phase6.placer

//backtrace
PB4transitions[50].currentState = 17; PB4transitions[50].currentChar = 8; PB4transitions[50].nextState = 17; PB4transitions[50].nextDirection = -1; PB4transitions[50].nextChar = 8;//voiture en main et sur tape alors continuer à revenir
PB4transitions[51].currentState = 17; PB4transitions[51].currentChar = 0; PB4transitions[51].nextState = 18; PB4transitions[51].nextDirection = 1; PB4transitions[51].nextChar = 8;//voiture en main mais pas sur tape alors placer et repartir en avant
PB4transitions[52].currentState = 18; PB4transitions[52].currentChar = 8; PB4transitions[52].nextState = 18; PB4transitions[52].nextDirection = 1; PB4transitions[52].nextChar = 8;//si toujours pas au centre continuer à repartir
PB4transitions[53].currentState = 18; PB4transitions[53].currentChar = 5; PB4transitions[53].nextState = 12; PB4transitions[53].nextDirection = 1; PB4transitions[53].nextChar = 5;//lorsqu'au centre, avancer et se mettre en phase7.handEmpty


//PHASE 6 :
//si ordre de vider
    //si centre
PB4transitions[54].currentState = 13; PB4transitions[54].currentChar = 6; PB4transitions[54].nextState = 14; PB4transitions[54].nextDirection = 1; PB4transitions[54].nextChar = 4;//si centre vider et passer en phase7.retour-handFull
    //sinon
PB4transitions[55].currentState = 13; PB4transitions[55].currentChar = 0; PB4transitions[55].nextState = 13; PB4transitions[55].nextDirection = -1; PB4transitions[55].nextChar = 0;//continuer
PB4transitions[56].currentState = 13; PB4transitions[56].currentChar = 3; PB4transitions[56].nextState = 13; PB4transitions[56].nextDirection = -1; PB4transitions[56].nextChar = 3;//continuer
PB4transitions[57].currentState = 13; PB4transitions[57].currentChar = 8; PB4transitions[57].nextState = 13; PB4transitions[57].nextDirection = -1; PB4transitions[57].nextChar = 8;//continuer
PB4transitions[58].currentState = 13; PB4transitions[58].currentChar = 7; PB4transitions[58].nextState = 13; PB4transitions[58].nextDirection = -1; PB4transitions[58].nextChar = 7;//continuer
    //revenir
PB4transitions[59].currentState = 14; PB4transitions[59].currentChar = 0; PB4transitions[59].nextState = 14; PB4transitions[59].nextDirection = 1; PB4transitions[59].nextChar = 0;//revenir
PB4transitions[60].currentState = 14; PB4transitions[60].currentChar = 3; PB4transitions[60].nextState = 14; PB4transitions[60].nextDirection = 1; PB4transitions[60].nextChar = 3;//revenir
PB4transitions[61].currentState = 14; PB4transitions[61].currentChar = 8; PB4transitions[61].nextState = 14; PB4transitions[61].nextDirection = 1; PB4transitions[61].nextChar = 8;//revenir
PB4transitions[62].currentState = 14; PB4transitions[62].currentChar = 7; PB4transitions[62].nextState = 14; PB4transitions[62].nextDirection = 1; PB4transitions[62].nextChar = 7;//revenir
    //si centre se mettre en phase3.handFull
PB4transitions[63].currentState = 14; PB4transitions[63].currentChar = 4; PB4transitions[63].nextState = 15; PB4transitions[63].nextDirection = 1; PB4transitions[63].nextChar = 4;//passer en phase7 avec full hand

//si ordre de rajouter
    //si centre
PB4transitions[64].currentState = 16; PB4transitions[64].currentChar = 4; PB4transitions[64].nextState = 19; PB4transitions[64].nextDirection = 1; PB4transitions[64].nextChar = 6;//si centre placer et passer en phase7.retour-handEmpty
    //sinon
PB4transitions[65].currentState = 16; PB4transitions[65].currentChar = 0; PB4transitions[65].nextState = 16; PB4transitions[65].nextDirection = -1; PB4transitions[65].nextChar = 0;//continuer
PB4transitions[66].currentState = 16; PB4transitions[66].currentChar = 3; PB4transitions[66].nextState = 16; PB4transitions[66].nextDirection = -1; PB4transitions[66].nextChar = 3;//continuer
PB4transitions[67].currentState = 16; PB4transitions[67].currentChar = 8; PB4transitions[67].nextState = 16; PB4transitions[67].nextDirection = -1; PB4transitions[67].nextChar = 8;//continuer
PB4transitions[68].currentState = 16; PB4transitions[68].currentChar = 7; PB4transitions[68].nextState = 16; PB4transitions[68].nextDirection = -1; PB4transitions[68].nextChar = 7;//continuer
    //revenir
PB4transitions[69].currentState = 19; PB4transitions[69].currentChar = 0; PB4transitions[69].nextState = 19; PB4transitions[69].nextDirection = 1; PB4transitions[69].nextChar = 0;//revenir
PB4transitions[70].currentState = 19; PB4transitions[70].currentChar = 3; PB4transitions[70].nextState = 19; PB4transitions[70].nextDirection = 1; PB4transitions[70].nextChar = 3;//revenir
PB4transitions[71].currentState = 19; PB4transitions[71].currentChar = 8; PB4transitions[71].nextState = 19; PB4transitions[71].nextDirection = 1; PB4transitions[71].nextChar = 8;//revenir
PB4transitions[72].currentState = 19; PB4transitions[72].currentChar = 7; PB4transitions[72].nextState = 19; PB4transitions[72].nextDirection = 1; PB4transitions[72].nextChar = 7;//revenir
    //si centre se mettre en phase3.handFull
PB4transitions[73].currentState = 19; PB4transitions[73].currentChar = 6; PB4transitions[73].nextState = 12; PB4transitions[73].nextDirection = 1; PB4transitions[73].nextChar = 6;//passer en phase7 avec empty hand

//PHASE 7   
//HandEmpty
PB4transitions[74].currentState = 12; PB4transitions[74].currentChar = 0; PB4transitions[74].nextState = 12; PB4transitions[74].nextDirection = 1; PB4transitions[74].nextChar = 0;//pas de voiture en main ni sur tape alors avancer
PB4transitions[75].currentState = 12; PB4transitions[75].currentChar = 8; PB4transitions[75].nextState = 15; PB4transitions[75].nextDirection = 1; PB4transitions[75].nextChar = 0;//pas de voiture en main mais sur tape alors ramasser

//HandFull
PB4transitions[76].currentState = 15; PB4transitions[76].currentChar = 0; PB4transitions[76].nextState = 12; PB4transitions[76].nextDirection = 1; PB4transitions[76].nextChar = 8;//voiture en main mais sur tape alors placer
PB4transitions[77].currentState = 15; PB4transitions[77].currentChar = 8; PB4transitions[77].nextState = 15; PB4transitions[77].nextDirection = 1; PB4transitions[77].nextChar = 8;//voiture en main et sur tape alors avancer

//PHASE 8
//Right border
PB4transitions[78].currentState = 12; PB4transitions[78].currentChar = 2; PB4transitions[78].nextState = 20; PB4transitions[78].nextDirection = -1; PB4transitions[78].nextChar = 2;//si border right alors commencer à revenir à gauche
PB4transitions[79].currentState = 15; PB4transitions[79].currentChar = 2; PB4transitions[79].nextState = 20; PB4transitions[79].nextDirection = -1; PB4transitions[79].nextChar = 2;//si border right alors commencer à revenir à gauche

//revenir
PB4transitions[80].currentState = 20; PB4transitions[80].currentChar = 0; PB4transitions[80].nextState = 20; PB4transitions[80].nextDirection = -1; PB4transitions[80].nextChar = 0;//tant que pas border gauche
PB4transitions[81].currentState = 20; PB4transitions[81].currentChar = 3; PB4transitions[81].nextState = 20; PB4transitions[81].nextDirection = -1; PB4transitions[81].nextChar = 3;//tant que pas border gauche
PB4transitions[82].currentState = 20; PB4transitions[82].currentChar = 4; PB4transitions[82].nextState = 20; PB4transitions[82].nextDirection = -1; PB4transitions[82].nextChar = 4;//tant que pas border gauche
PB4transitions[83].currentState = 20; PB4transitions[83].currentChar = 5; PB4transitions[83].nextState = 20; PB4transitions[83].nextDirection = -1; PB4transitions[83].nextChar = 5;//tant que pas border gauche
PB4transitions[84].currentState = 20; PB4transitions[84].currentChar = 6; PB4transitions[84].nextState = 20; PB4transitions[84].nextDirection = -1; PB4transitions[84].nextChar = 6;//tant que pas border gauche
PB4transitions[85].currentState = 20; PB4transitions[85].currentChar = 7; PB4transitions[85].nextState = 20; PB4transitions[85].nextDirection = -1; PB4transitions[85].nextChar = 7;//tant que pas border gauche
PB4transitions[86].currentState = 20; PB4transitions[86].currentChar = 8; PB4transitions[86].nextState = 20; PB4transitions[86].nextDirection = -1; PB4transitions[86].nextChar = 8;//tant que pas border gauche

//Border gauche = recommencer
PB4transitions[87].currentState = 20; PB4transitions[87].currentChar = 1; PB4transitions[87].nextState = 21; PB4transitions[87].nextDirection = 0; PB4transitions[87].nextChar = 1;//printState puis repartir
PB4transitions[88].currentState = 21; PB4transitions[88].currentChar = 1; PB4transitions[88].nextState = 0; PB4transitions[88].nextDirection = 1; PB4transitions[88].nextChar = 1;//printState puis repartir



int PB4transitionsSize = 89;
int PB4headPosition = 1;

printParameters* PB4printParams = malloc(sizeof(printParameters));
int PB4fileCount = 10000;
PB4printParams->fileCount = &PB4fileCount;
PB4printParams->leftBorderChar = 1;
PB4printParams->rightBorderChar = 2;
PB4printParams->printState = 21;


// en fonction des tests qu'on veut
//test simple

int PB4startingTape[65] = {1,7,7,0,0,7,7,7,7,0,0,0,7,7,7,0,4,0,0,0,7,7,7,7,7,0,0,0,0,0,0,0,3,0,0,0,8,8,8,8,8,8,8,0,0,0,0,0,4,0,0,0,0,0,0,8,8,8,8,0,0,0,0,0,2};
PB4printParams->OutputDir = "CrossRoadTest";
int PB4startingTapeLength = 65;

//accumulation
/*
int PB4startingTape[305] = {1,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,7,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,8,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2};
int PB4startingTapeLength = 305;
PB4printParams->OutputDir = "CrossRoadTestDots";
*/




turing* PB4machine = makeTuringMachine(PB4alphabetSize, PB4blank, PB4stateSize, PB4starterState, PB4finalStates, PB4finalStatesSize, PB4transitions, PB4transitionsSize, PB4startingTape, PB4startingTapeLength, PB4headPosition);
//runMachine(PB4machine);
//runMachineWithOptionalPrint2D(PB4machine, PB4printParams, 100);

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

return 0;
}