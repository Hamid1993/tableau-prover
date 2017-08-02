//
//  constants.h
//  
//
//  Created by Hamid Doust on 02.08.17.
//
//

#ifndef constants_h
#define constants_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define ERROR_VALUE -1
#define IS_PROPOSITION 1
#define IS_NEGATION 2
#define IS_BINARY 3
#define NOT_A_FORMULA 0



// an upper bound for the amount of characters a single formula can have
// edit this variable if you use formulas with more characters than 50
int MAXIMUM_SIZE_OF_FORMULA=50;

/*typedef struct tableau tableau;*/

struct tableau {
    char *root;
    struct  tableau *left;
    struct tableau *right;
    struct tableau *parent;
    
    // determines if the node in the tableau was fully processed
    // 1 means it was fully processed, 0 means it wasnt
    int finished ;
}*tab, *node, *node1, *kid, *pa;



/**
 * gives information about a binary formula
 * is used in order to split the formula to two subformulas (SplitFormula - struct)
 */
typedef struct {
    // the index of the operator that connects the two parts of the formula
    int binaryOperatorIndex;
    // the occurence of the last bracket within the formula
    int lastBracketIndex;
}BinaryInfo ;

/**
 * is used to split a formula into two subformulas
 */
typedef struct {
    char * firstFormula;
    char * secondFormula;
    char operator;
}SplitFormula;



struct symbolNode{
    char symbol;
    int value;
    struct symbolNode * next;
};



#endif /* constants_h */
