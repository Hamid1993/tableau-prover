//
//  main.c
//  coursework
//
//  Created by Hamidulah Doust on 28.12.16.
//  Copyright © 2016 Hamidulah Doust. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define ERROR_VALUE -1
#define IS_PROPOSITION 1
#define IS_NEGATION 2
#define IS_BINARY 3
#define NOT_A_FORMULA 0




int Fsize=50;
int cases=10;


int i;
int j;



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



// returns 1 if c is a proposition, otherwise 0
int isProposition(char c) {
    if (c == 'p' || c == 'q' || c == 'r') {
        return 1;
    } else {
        return 0;
    }
}

// returns a Splitformula struct initialized in the heap with the passed parameters
SplitFormula * initializeSplitFormula(char * firstFormula, char * secondFormula, char operator) {
    
    // init memory for the struct
    SplitFormula * formula = malloc(sizeof(SplitFormula));
    
    // init memory for the contents
    formula->firstFormula = malloc(sizeof(char) * strlen(firstFormula));
    
    // copy the values
    strcpy(formula->firstFormula, firstFormula);
    
    // assign operator
    formula->operator = operator;
    
    
    // check if the secondFormula is NULL, this could be the case when formula starts with two negations (alpha rule)
    // copy the values
    if (secondFormula != NULL) {
        formula->secondFormula = malloc(sizeof(char) * strlen(secondFormula));
        strcpy(formula->secondFormula, secondFormula);
    } else {
        formula->secondFormula = NULL;
    }
    return formula;
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



// only gets invoked if the formula encoded as string is correct and starts with a bracket (index 0)
// returns information about the operator and the occurence of the  last bracket so that it is possible to
// create two subformulas from the info and the string
BinaryInfo createBinaryInfoFromBinaryBracketFormula (char *g)  {
    int lastBracketIndex = -1;
    int indexOfOperator = -1;
    
    
    // number of formulas read (since b is a binary it consists exactly out of two formulas)
    int countFormulas = 0;
    
    // one because of index 0
    int leftBrackets = 1;
    int rightBrackets = 0;
    
 for(int i = 1; g[i] != '\0'; i++) {
     // get index of operator
     
     if (g[i] == '(') {
         leftBrackets++;
     }
     
     else if (g[i] == ')') {

         rightBrackets++;
         
         if (leftBrackets - rightBrackets == 1 && countFormulas == 0) {
             
             // the next index must be the operator
             indexOfOperator = i+1;
             countFormulas++;
             
         } else if (leftBrackets - rightBrackets == 0) {
             // the current index mast be the index of the last bracket
             lastBracketIndex = i;
             break;
            }
        }
     
    else if (validLiteral(g, i) && leftBrackets == 1 && rightBrackets == 0 && countFormulas == 0) {
        
        indexOfOperator = validLiteral(g, i);
        countFormulas++;
     
    }
     
     
     
     
     
     
 }
    
    BinaryInfo * info = malloc(sizeof(BinaryInfo));
    info->binaryOperatorIndex = indexOfOperator;
    info->lastBracketIndex = lastBracketIndex;
    
    return *info;

}

// create an string from the startIndex to the endIndex (both including)
// copies endIndex - startIndex + 1 symbol from oldformula to a new one
char * createFormulaFromFormula(char * oldFormula, int startIndex, int endIndex) {
    
    // add one to create a null terminated string
    // copy one more symbol for the null terminater
    char * newFormula = malloc(sizeof(char)*(endIndex - startIndex + 2));
    
    // copy the formula and terminates it with the null terminator
    stpncpy(newFormula, oldFormula + startIndex, endIndex - startIndex + 1);
    
    return newFormula ;
    
    
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
        // check if the next symbol is a literal, if so valid contains the index after the literal, otherwise 0
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
            
                // set i to index-1 because the index gets incremented anyway
              //  i = index;
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

// splits a formula into two formulas, the formula to be splitted must be a binary formula and must start with a bracket
// returns also the operator that was splitted
SplitFormula * binaryProcessorWithBrackets(char * g) {
    
    // since the formula is binary it must start with an open bracket
    BinaryInfo info = createBinaryInfoFromBinaryBracketFormula(g);
    
    // create two substrings with the new formulas
    // index characters are included
    // start from 1 because we do not want the starting bracket to be included
    
    // distinguish in dependency of the different operators
    
    char * firstFormula;
    char newOperator ;
    if (g[info.binaryOperatorIndex] == '>') {
        
        // change operator
        newOperator = 'v';
        
        // add one additional for the null terminator symbol, since we are not copying the operator its only
        // info.binaryOperatorIndex many symbols, but we need to put a negation as first symbol and a null terminator as additional symbol
        firstFormula = malloc(sizeof(char)*(info.binaryOperatorIndex + 2));
        firstFormula[0] = '-';
        // we want to copy g from the second symbol till the operator excluding (the first symbol of g is a bracket)
        // the first symbol of our first formula is '-'
        stpncpy(firstFormula + 1 , g + 1 , info.binaryOperatorIndex -1);

    } else {
        
        // operator still the same
        newOperator = g[info.binaryOperatorIndex];
        
        firstFormula = createFormulaFromFormula(g, 1, info.binaryOperatorIndex-1);

    }
    
    
    char * secondForumla = createFormulaFromFormula(g, info.binaryOperatorIndex +1, info.lastBracketIndex-1);
    
    // init memory for the struct
    SplitFormula * formula = initializeSplitFormula(firstFormula, secondForumla, newOperator);
    
    
    return formula;
    
    
    
}



// gets invoked if the formula is a negation followed by a bracket
// (no double negation and not a negated literal only (because we would be finished))
// returns the two negated subformulas and the negated operator in a struct
SplitFormula * negationProcessorWithBrackets(char * g) {
    
        // change the pointer to point to the bracket as first symbol
        SplitFormula *formula = binaryProcessorWithBrackets(g+1);
    
    
    // allocate one extra char for the null terminator
    char * newFirstFormula = malloc(sizeof(char) * (strlen(formula->firstFormula)+2));
    char * newSecondFormula = malloc(sizeof(char) * (strlen(formula->secondFormula)+2));

    // both formulas start with a negation
    newFirstFormula[0] = '-';
    newSecondFormula[0] = '-';

    // copy the formulas
    strcpy(newFirstFormula +1, formula->firstFormula);
    strcpy(newSecondFormula +1, formula->secondFormula);


    // negate the operator
    char newOperator;
    
            
    if (formula->operator == '^') {
          newOperator = 'v';
    } else {
        // handle both cases: 'v' and '>'
        newOperator = '^';
    }
    SplitFormula * newFormula = initializeSplitFormula(newFirstFormula, newSecondFormula, newOperator);
    return newFormula;
    
}


// returns 1 if the formula is a proposition or a negated proposition, otherwise 0
int isFinished(char * formula) {
    if ((isProposition(formula[0]) && formula[1] == '\0')||(formula[0] == '-' && isProposition(formula[1]) && formula[2] =='\0')) {
        return 1;
    } else {
        return 0;
    }
}

// only gets invoked if the first two symbols of the formular are negations
// this method deletes both negations
char * doubleNegationProcessor (char * formulaString) {
    
    unsigned long lengthOfString = strlen(formulaString);
    // add one additional character for the null terminator
    char * newFormula = calloc(1, (sizeof(char) *(lengthOfString - 1)));
    
        for(int i = 0; i < lengthOfString - 2; i++) {
            newFormula[i] = formulaString[i + 2];
        }
    return newFormula;
}

                                  


// only gets invoked if the formula is correct and we are not finished yet
SplitFormula * formularProcessor (char * formularString) {
    
    SplitFormula * formula = NULL;
    
    // if the formular starts with an open bracket
    if (formularString[0] == '(') {
        formula = binaryProcessorWithBrackets(formularString);
    } else if (formularString[0] == '-' && formularString[1] == '('){
        // negation that starts with an open bracket
        formula = negationProcessorWithBrackets(formularString);
        
    } else {
        // since we are not finished yet and there is no starting bracket this means there is exactly one literal with at least two negations (othwise we would be finished)
        char * newFormula = doubleNegationProcessor(formularString);
        
        // pass NULL and an invalid operator as argument to indicate a double negation
        formula = initializeSplitFormula(newFormula, NULL, '\0');
        
    }
    
    return formula;
}


// a child to a specific leave of the tree in dependency of the used operator
void addChildrenToOneLeave(struct tableau * leave, struct tableau * leftChild, struct tableau * rightChild, char operator ) {
    
    if (operator == 'v') {
        
        // add both children
        leave->left = leftChild;
        leave->right = rightChild;
        
        // connect children to parent
        leftChild->parent = leave;
        rightChild->parent = leave;
        
    } else if (operator == '^') {
        
        // alpha formula
        leave->left = leftChild;
        leftChild->left = rightChild;
        
        leftChild->parent = leave;
        rightChild->parent = leftChild;
        
        
    } else {
        // if the operator is NULL, this is the case if the original formula contains a double negation
        // the rightChild is also NULL in this case
        
        leave->left = leftChild;
        leftChild->parent = leave;
    }
}

// initialize a tableau with formula on the heap and copy the char also
struct tableau * initializeTableauOnHeap(char * formular) {
    
    
    if (formular == NULL) {
        return NULL;
    }
    
    // use calloc to get a block full of zeros otherwise we would need to do this explictly (because we want to use NULL checks)
    struct tableau * t = calloc(1, sizeof(struct tableau));
    
    // init memory for the root formula and assigned the free memory to root
    t->root =  malloc(sizeof(char) * strlen(formular));
    
    // copy the formula to root
    strcpy(t->root, formular);
    
    
    
    
    return t;
    
}


// add the children to all leaves of the current subTree, use depth first search
// left child is null <-> we found a leave
void addChildrenToAllLeavesOfTree(struct tableau * tree, struct tableau * leftChild, struct tableau * rightChild, char operator) {
    struct tableau * leftIterator = tree;
    struct tableau * rightIterator = tree;
    
    if (leftIterator->left != NULL) {
        leftIterator = leftIterator->left;
        
        // invoke it recursively to find a leave
        addChildrenToAllLeavesOfTree(leftIterator, leftChild, rightChild, operator);
        
    } else {
        // found leave
        // add children
        
        
        // end the recursion here because we already found the leave
        
        // create new children on the heap for each leave first, otherwise we would use the same children for each leave
        // (this would be wrong then, because we would add only once the children for one leave effectively
        // copy the childs on the heap

        // the left child cant be NULL
        struct tableau * copyOnHeapLeftChild = initializeTableauOnHeap(leftChild->root);
        struct tableau * copyOnHeapRightChild = NULL;
        
        // rightChild can be NULL in case of a double negation, prevent a memory access error by checking it
        if (rightChild != NULL) {
        copyOnHeapRightChild = initializeTableauOnHeap(rightChild->root);
        }
        
        // add to the tree
        return addChildrenToOneLeave(leftIterator, copyOnHeapLeftChild, copyOnHeapRightChild, operator);
        
    }
    
    if(rightIterator->right != NULL) {
        rightIterator = rightIterator->right;
        
        // invoke it recursively for the new node to find a leave
        addChildrenToAllLeavesOfTree(rightIterator, leftChild, rightChild, operator);

    }
    
}





// processes the current node of the tableau, creates new branches in dependency of the formula in the node
// and applies those rules (alpha / beta rules)
// gets only invoked if the node of the tableau is not finished yet (formula not finished)
void processTableauNode(struct tableau* t) {
    
    // processes the current node of the tableau, returns a splitformula on the heap
    SplitFormula * splitFormula = formularProcessor(t->root);

   
    // create a new tableau nodes on the heap, the char of the splitformula gets also copied on the heap
    // for each tableau
    struct tableau * firstNode = initializeTableauOnHeap(splitFormula->firstFormula);
    struct tableau * secondNode = initializeTableauOnHeap(splitFormula->secondFormula);


    // add the two children to all leaves in dependency of the operator
    addChildrenToAllLeavesOfTree(t, firstNode, secondNode, splitFormula->operator);
    
    // free the memory of the splitformula and from the two tableau nodes, since we have created own copies in addChildrenToAllLeaves and do not need them anymore
    free(splitFormula);
    free(firstNode);
    free(secondNode);
    
    
}



// gets invoked if the node is seen the first time
void depthFirstSearch(struct tableau * t) {
    
    // check if we are finished with this node
    // a node is finished if both children are finished (if they exist)
    // a node without children is finished if the root formula of that node
    // is a proposition or a negated proposition
    
    if(t->finished == 0) {
        // not finished yet
        
        // check if the formula in the current node is finished, if so no need to process it
        // (but it is still possible that the children need to be processed because of the alpha rules
        if (isFinished(t->root) == 0)
        {
            // processes the formula and add children
            processTableauNode(t);
        }
        
        // check if the children exist and needs to be processed
        if (t->left != NULL) {
            depthFirstSearch(t->left);
        }
        
        // check if the right child exist, if so process it
        if (t->right != NULL) {
            depthFirstSearch(t->right);
        }
        
        // mark node as finished because we have processed both children
        t->finished = 1;
        
    }
    
}


// only gets invoked if the table is initialzied with a correct formula
void complete(struct tableau *t) {
    
// use depth first search to complete the tableau t
    depthFirstSearch(t);
}



// returns 0 if symbol  = 'r', 1 if symbol = 'p', 2 if symbol = 'q'
int returnSymbolValue(char symbol) {
    
     if (symbol == 'r') {
         return 0;
    }
    if (symbol =='p'){
        return 1;
        
    } else  {
        // must be q
        return 2;
        
    }
    
}

// returns 1 if the branch is open, 0 if it is closed
// gets a leave and works it way up to the root
int checkIfBranchIsOpen(struct tableau* leave) {
    
    // use two boolean arrays, one tracks the occurences of propositions, the other one of negated propositions where 1 means "occured" and 0 means "not occured"
    int  propositions [] = {0,0,0};
    int negatedPropositions [] = {0,0,0};
    // since we have only three possible propositions r, p, q
    // use the array to determine if the branch is satisfiable

    
    struct tableau * current = leave;
    
    do {
    
        // check if the current formula is a proposition or a negated proposition
        if(isFinished(current->root)) {
            
            if(current->root[0] == '-' ) {
                
                int index = returnSymbolValue(current->root[1]);
                // mark that this negated proposition has occured
                // it doesnt matter how often a proposition/ negated proposition occurs
                negatedPropositions[index] = 1;
                
            } else {
                int index = returnSymbolValue(current->root[0]);
                // mark that this proposition has occured
                propositions[index] = 1;
            }
        }
        
         // go one to the top
        current = current->parent;
            
        
    }while (current != NULL);
    
    // at first mark it as satisfiable, now compare if both arrays have an index where they both contain
    // the value 1, if so this means this branch is not satisfiable
    int returnValue = 1;
    
    // check if the array only contains valid values
    for(int i = 0; i < 3; i++) {
        
        // check if both are one
        if(propositions[i]&& negatedPropositions[i]) {
            returnValue = 0;
            break;
        }
    }
    return returnValue;
    
}


// checks if the tableau is closed by finding all leaves and check for each leave if the branch is open
// returns 1 if the tableau is closed, 0 otherwise
int closed(struct tableau * t) {
    
    
    
    if(t->left != NULL) {
        
        // find leave
        // just change the returnvalue in case a branch is found because we are finished then
        if(!closed(t->left)) {
             return 0;
        }
    } else {
        
        // found leave
        if(checkIfBranchIsOpen(t)) {
            // found one branch
             return 0;

        }
    }
    
    if (t->right != NULL) {
        
        // find leave
        if(!closed(t->right)) {
            return 0;
        }
    }
    
    
    return 1;
    
}

int countNumberOfLinesOfFile(char *fileName) {

FILE* inputFile = fopen(fileName, "r");
int nextCharacter;
int numberOfLines;

do 
{
    nextCharacter = fgetc(inputFile);
    if(nextCharacter == '\n')
        numberOfLines++;
} while (nextCharacter != EOF);

// last line doesn't end with a new line!
// but there has to be a line at least before the last line
if(nextCharacter != '\n' && numberOfLines != 0) 
    numberOfLines++;

fclose(inputFile);

return numberOfLines;

}


int main()

{ /*input a string and check if its a propositional formula */
    
    
    char *name = malloc(Fsize);
    FILE *fp, *fpout;

    char * fileInputName = "input.txt";
    char * fileOutputName = "output.txt";
    
    /* reads from input.txt, writes to output.txt*/
    if ((  fp=fopen(fileInputName,"r"))==NULL){printf("Error opening file");exit(1);}
    if ((  fpout=fopen(fileOutputName,"w"))==NULL){printf("Error opening file");exit(1);}
    
    // count number of lines of the file
    int numberOfLines = countNumberOfLinesOfFile(fileInputName);
   /*
    for(int j=0;j<numberOfLines;j++)
    {
        fscanf(fp, "%s",name);//read formula
        switch (parse(name))
        {case(0): fprintf(fpout, "%s is not a formula.  ", name);break;
            case(1): fprintf(fpout, "%s is a proposition.  ", name);break;
            case(2): fprintf(fpout, "%s is a negation.  ", name);break;
            case(3):fprintf(fpout, "%s is a binary.  ", name);break;
            default:fprintf(fpout, "What the f***!  ");
        }
        
        //make new tableau with name at root, no children, no parent
        
        struct tableau t={name, NULL, NULL, NULL};
        
        //expand the root, recursively complete the children
        if (parse(name)!=0)
        { complete(&t);
            if (closed(&t)) fprintf(fpout, "%s is not satisfiable.\n", name);
            else fprintf(fpout, "%s is satisfiable.\n", name);
        }
        else fprintf(fpout, "I told you, %s is not a formula.\n", name);
    }*/
    printf("Number of lines: %d\n",numberOfLines );    

    
    fclose(fp);
    fclose(fpout);
    free(name);
    
    return(0);
}
