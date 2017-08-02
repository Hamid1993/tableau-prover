//
//  completeTableau.h
//  
//
//  Created by Hamid Doust on 02.08.17.
//
//

#ifndef completeTableau_h
#define completeTableau_h
#include "constants.h"
void complete(struct tableau *t);
void depthFirstSearch(struct tableau * t);
void processTableauNode(struct tableau* t);
struct tableau * initializeTableauOnHeap(char * formular);

void addChildrenToAllLeavesOfTree(struct tableau * tree, struct tableau * leftChild, struct tableau * rightChild, char operator);

void addChildrenToOneLeave(struct tableau * leave, struct tableau * leftChild, struct tableau * rightChild, char operator );

SplitFormula * formularProcessor (char * formularString);
char * doubleNegationProcessor (char * formulaString);
SplitFormula * negationProcessorWithBrackets(char * g);
SplitFormula * initializeSplitFormula(char * firstFormula, char * secondFormula, char operator);
BinaryInfo createBinaryInfoFromBinaryBracketFormula (char *g);
char * createFormulaFromFormula(char * oldFormula, int startIndex, int endIndex);
SplitFormula * binaryProcessorWithBrackets(char * g);

#endif /* completeTableau_h */
