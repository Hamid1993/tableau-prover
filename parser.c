//
//  parser.c
//  
//
//  Created by Hamid Doust on 02.08.17.
//
//

#include "parser.h"



/*
 * returns 1 if a proposition, 2 if neg, 3 if binary, ow 0
 * a proposition is per definition just a letter in our case, not a negated letter (slides)
 */
int parse(char *g) {
    
    int valid =  correctFormula(g);
    if (valid == ERROR_VALUE) {
        return NOT_A_FORMULA;
        
    } else {
        
        if(isProposition(g[0])&& g[1] =='\0') {
            return IS_PROPOSITION;
            
        } else if ( g[0] == '-') {
            return IS_NEGATION;
            
        } else {
            // if it is a formula and it is not a proposition and not a negation it must be a binary
            return IS_BINARY;
        }
    }
    
}


// return a positive number in case of success (the formula is syntactically correct)
int correctFormula(char *g) {
    
    
    
    // first check the case if the string consists only of one literal (with arbitrary many negations)
    // index is the first index after the literal
    int index = validLiteral(g, 0);
    
    if (g[index] =='\0') {
        return 1;
    }
    
    // check if the string only contains valid symbols
    // check for parenthesis matching
    // check if before each binary
    for(int i = 0; g[i] != '\0'; i++) {
        
        // they are allowed, so ignore them for  now
        if(g[i] == '-' ) {
            continue;
        }
        
        
        if (g[i] == '(') {
            // call validBracket
            int newIndex = validBracket(g, i+1);
            // check if string is finished
            if (newIndex == ERROR_VALUE) {
                return ERROR_VALUE;
            } else if (g[newIndex] == '\0') {
                // is a correct formula
                return 1;
            } else {
                return ERROR_VALUE;
            }
        }
        
        
        else {
            return ERROR_VALUE;
        }
    }
    return ERROR_VALUE;
}

// return new index after the literal if the index is a literal (with arbitrary many negations), otherwise 0
int validLiteral(char *g, int startIndex) {
    // allowed literals: p, q, r
    
    for(int i = startIndex; g[i] != '\0'; i++) {
        
        // is allowed
        if(g[i] == '-') {
            continue;
        }
        
        // only literals that are allowed:
        // p,q
        if (isProposition(g[i])) {
            // return new index after the literal
            return i + 1;
        } else {
            // no literal, return errrorcode 0 (= false)
            return 0;
        }
        
    }
    // only reaches the end if the string is finished
    return 0;
}



// returns new index (index after closing bracket) if successful, otherwise -1
// check if the startIndex of an opening bracket is
// the starting point for a valid bracket within a formula
int validBracket(char *g, int startIndex) {
    int formulaCounter = 0;
    int operatorCounter = 0;
    // check if the string only contains valid symbols
    // check for parenthesis matching
    // check if before each binary
    for(int i = startIndex; g[i] != '\0'; i++) {
        if(g[i] == '-') {
            continue;
        }
        // check if the next symbol is a literal, if so indexAfterLiteral contains the index after the literal, otherwise 0
        int indexAfterLiteral = validLiteral(g,i);
        if (g[i] == '(') {
            // handle error case
            if (formulaCounter > 1 || (formulaCounter == 1 && operatorCounter == 0)) {
                // one bracket must exactly connect two formulas with an operator
                return ERROR_VALUE;
            }
            
            
            int index = validBracket(g, i+1);
            if(index == ERROR_VALUE) {
                // indicate that this is not a valid formula
                return ERROR_VALUE;
                
            }
            formulaCounter++;
            
            // check if we are finished
            if (formulaCounter == 2 && operatorCounter == 1) {
                // we are expecting a closed bracked
                if (g[index] != ')') {
                    return ERROR_VALUE;
                } else {
                    return index+1;
                }
            }
            
            // set i to index-1 because the index gets incremented anyway
            i = index - 1 ;
            
            
            
            // check if the next symbol is a literal
        } else if (indexAfterLiteral) {
            
            
            if (formulaCounter == 0) {
                formulaCounter++;
                
                // update index, add minus 1 because the index will be incremented for the next loop
                i = indexAfterLiteral-1;
                continue;
                
            } else if (formulaCounter == 1 && operatorCounter == 1) {
                // finished we are expecting a bracket after this
                if (g[indexAfterLiteral] != ')') {
                    return ERROR_VALUE;
                } else {
                    return indexAfterLiteral + 1;
                }
                
            }
            
            
        } else if (validOperator(g, i) && formulaCounter == 1 && operatorCounter == 0) {
            
            // we are expecting
            operatorCounter++;
            continue;
            
            
            
        } else {
            return ERROR_VALUE;
        }
        
        
    }
    
    // in case
    return ERROR_VALUE;
    
    
}

// returns 1 if the formula is a proposition or a negated proposition, otherwise 0
int isFinished(char * formula) {
    if ((isProposition(formula[0]) && formula[1] == '\0')||(formula[0] == '-' && isProposition(formula[1]) && formula[2] =='\0')) {
        return 1;
    } else {
        return 0;
    }
}


// return the index the proposition has in the list, if the proposition doesnt exist yet add it
int getIndexFromProposition(char proposition) {
    return returnSymbolValue(proposition, -1);
}



// return the proposition which belongs to the index, returns -1 if the index is invalid
char getPropositionFromIndex(int index) {
    if(index < 0) {
        return -1;
    }
    return (char) returnSymbolValue('a', index);
}

// returns 1 if the next symbol is a proposition, otherwise 0
int isProposition(char symbol) {
    static char specialSymbols [] = {'^','v','<','>','(',')','-'};
    
    for (int i = 0; i < sizeof(specialSymbols)/sizeof(char); i++) {
        if (specialSymbols[i] == symbol) {
            return 0;
        }
    }
    
    // this symbol is a proposition, so store it
   	getIndexFromProposition(symbol);
    return 1;
}

// returns the index after the operator, if the startIndex points to a valid operator, otherwise 0
int validOperator(char * g, int startIndex) {
    
    // only operators that are allowed:
    // ^,>,<,v
    if (g[startIndex] == '^' || g[startIndex] == '>' || g[startIndex] == '<' || g[startIndex] == 'v') {
        // return new index after the literal
        return startIndex + 1;
    } else {
        // no valid operator, return errrorcode 0 (= false)
        return 0;
    }
    
}




