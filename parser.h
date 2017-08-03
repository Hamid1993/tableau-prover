//
//  parser.h
//  
//
//  Created by Hamid Doust on 02.08.17.
//
//

#ifndef parser_h
#define parser_h
#include "constants.h"
#include "helperMethods.h"
int validBracket(char *g, int startIndex);
int parse(char *g);
int correctFormula(char *g);
int validLiteral(char *g, int startIndex);
int validBracket(char *g, int startIndex);
int isFinished(char * formula);
int getIndexFromProposition(char proposition);
char getPropositionFromIndex(int index);
int isProposition(char symbol);
int validOperator(char * g, int startIndex);

#endif /* parser_h */
