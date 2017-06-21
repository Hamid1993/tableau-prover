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

struct symbolNode* createSymbolNode(char symbol, int value) {

      // doesnt exist yet, so add new symbol
    struct symbolNode * newSymbol = malloc(sizeof(struct symbolNode));
    newSymbol->symbol = symbol;
    newSymbol->value = value;
    newSymbol->next = NULL; 
    return newSymbol;
}




// this method gets not used directly
// use the methods: getTotalNumberOfPropositions, getIndexFromProposition, getPropositionFromIndex instead
// if the symbol hasnt been seen before it adds it to the list
// if additionalParameter is -1 , ignore it, meaning insert it into the list and return the value / look the value up if it is in the list
// if additionalParameter is -2 ,return the total number of symbols
// if additionalParameter has another value, return the ASCII CODE for the symbol which belongs to the provided value, -1 if it doesnt exist
int returnSymbolValue(char symbol, int additionalParameter) {

// init once
static int counter = 0;
static struct symbolNode * list = NULL;


// check what kind of information is needed by the invoker
if (additionalParameter == -2) {
    // only return the total number of symbols seen so far
    return counter;
} else if (additionalParameter != -1) { 
	// in this case we want to return the ASCII symbol code for a provided value if it is stored, -1 otherwise
	struct symbolNode * current = list;
	while (current!= NULL) {
		if (current->value == additionalParameter) {
			return current->symbol;
		}
		current = current->next;
	}
	// return -1 indicating that there is no entry for this value stored
	return -1;

} else {
	// in this case the additionalParameter is set to -1, i.e. ignore the value 
    // user wants the number of a specific symbol
    // check if the list is empty
    if (list == NULL) {
    // update head
    list = createSymbolNode(symbol, counter);
    // update counter
    counter++;
    return counter-1;

    }

    // check if a node for this symbol already exist
    struct symbolNode * firstNode = list;
    struct symbolNode * secondNode = firstNode->next;



    while (secondNode != NULL) {
        if (firstNode->symbol == symbol) {
            return firstNode->value;
        }
        firstNode = secondNode;
        secondNode = secondNode->next;
    }

    // must check if the last node contains the symbol
    if (firstNode->symbol == symbol) {
        return firstNode->value;
    } 

    // doesnt exist yet, so add new symbol node
    struct symbolNode * newSymbol = createSymbolNode(symbol, counter);

    // insert into the end of the list
    firstNode->next = newSymbol;
    // update counter
    counter++;

    //return counter-1;
    //return newSymbol->value;
}
    
}

// returns the total number of propositions seen so far
int getTotalNumberOfPropositions() {
	return returnSymbolValue('a', -2);
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



void printCurrentTable() {
	// get number of stored symbols first
	int total = getTotalNumberOfPropositions();

	char values [total];

	for(int i = 0; i < total; i++ ) {
		values[i] = (char) getPropositionFromIndex(i);
		printf("Value with index %d is %c \n",i, values[i] );
	}




	}



// creates a possible solution for a proposition in form of a tableau, which represents the leave
// if the first entry of the array contains -1 then the solution is invalid, otherwise valid
int * createSolutionVectorForLeave(struct tableau* leave) {
    
    // use two boolean arrays, one tracks the occurences of propositions, the other one of negated propositions where 1 means "occured" and 0 means "not occured"
    // -2 indicates that we want to have the number of symbols stored so far
    int numberOfSymbols = getTotalNumberOfPropositions();
    int  propositions [numberOfSymbols] ;
    int negatedPropositions [numberOfSymbols] ;
    memset( propositions, 0, numberOfSymbols*sizeof(int) );
	memset( negatedPropositions, 0, numberOfSymbols*sizeof(int) );


    int * solutionvector  = (int *) malloc(sizeof(int)*numberOfSymbols);

    
    struct tableau * current = leave;
    
    do {
        // check if the current formula is a proposition or a negated proposition
        if(isFinished(current->root)) {
            
            if(current->root[0] == '-' ) {
                
                int index = getIndexFromProposition(current->root[1]);
                // mark that this negated proposition has occured
                // it doesnt matter how often a proposition/ negated proposition occurs
                negatedPropositions[index] = 1;
                
            } else {
                int index = getIndexFromProposition(current->root[0]);
                // mark that this proposition has occured
                propositions[index] = 1;
            }
        }
        
         // go one to the top
        current = current->parent;
            
        
    }while (current != NULL);
    
    int errorValue = 0;
    
    // check if the array only contains valid values
    for(int i = 0; i < numberOfSymbols; i++) {
        
        // check if both are one
        if(propositions[i]&& negatedPropositions[i]) {
            errorValue = 1;
            break;
        } else { 

            // complete solution vector, this is one possible solution 
            if (negatedPropositions[i] == 1) {
                solutionvector[i] = 0;
            } else {
                solutionvector[i] = 1;
            }
        }
    }

    // check if solution is valid, otherwise first entry contains -1
    if (errorValue == 1) {
        solutionvector[0] = -1;
    }

    return solutionvector;
    
}



// creates a possible solution for a proposition in form of a tableau, which represents the root
// if the first entry of the array contains -1 then the solution is invalid, otherwise valid
int * createSolutionVectorForRoot(struct tableau * t) {

    int totalNumOfSymbols = getTotalNumberOfPropositions();
    int * leftSolutionVector = NULL;
    int * rightSolutionVector = NULL;

    // if the left is null this implies the right is null in this construction of a binary tree
    if(t->left != NULL) {
       leftSolutionVector = createSolutionVectorForRoot(t->left);
    } else {

        // found leave, is left branch is null the right must be null as well
        return createSolutionVectorForLeave(t);
        
    }
    
    if (t->right != NULL) {
       rightSolutionVector = createSolutionVectorForRoot(t->right);
    } 
    
    // check if at least one solution is valid
    if (leftSolutionVector[0]!= -1 || rightSolutionVector == NULL) {
    	// free other memory which is not needed
    	if (rightSolutionVector != NULL) {
    		free(rightSolutionVector);
    	}

        return leftSolutionVector;
    } else {
    	// the left solution vector always has a value which is not NULL
    	free(leftSolutionVector);
        return rightSolutionVector;
    } 
}
    
// delete all children, need to free the memory of t yourself
void deleteTree(struct tableau * t) {
	
	// delete tree in post order style
	if (t->left != NULL) {
		deleteTree(t->left);
	}

	if(t->right != NULL) {
		deleteTree(t->right);
	}



}



int main(int argc, char * argv[])

{ /*input a string and check if its a propositional formula */
    
    if(argc < 2) {
    	// user failed to provide an input file
    	printf("Please provide at least input file !");
    	exit(1);

    }
    //store the temporary formula in this variable
    char *name = malloc(MAXIMUM_SIZE_OF_FORMULA);
    FILE *fp, *fpout;

    char * fileInputName = argv[1];
    char * fileOutputName = argc == 3 ? argv[2] : "output.txt";
    
    /* reads from input.txt, writes to output.txt*/
    if ((  fp=fopen(fileInputName,"r"))==NULL){printf("Error opening file");exit(1);}
    if ((  fpout=fopen(fileOutputName,"w"))==NULL){printf("Error opening file");exit(1);}
    int lineCounter = 1;
     while(fscanf(fp, "%s",name) != EOF) {
    	
        //make new tableau with name at root, no children, no parent
        
       struct tableau t = {name, NULL, NULL, NULL};
        //expand the root, recursively complete the children
       fprintf(fpout, "---------Line %d-------------\n",lineCounter);
        if (parse(name)!=0)
        { 
        	// process the tableau
        	complete(&t);

        	// get a solution vector and look up if it is valid
        	int * solutionvector = createSolutionVectorForRoot(&t);
        	if(solutionvector[0] != -1) {

        	int totalNumOfSymbols = getTotalNumberOfPropositions();
			int stringSolution [(totalNumOfSymbols*4+1)] ;
			    fprintf(fpout, "\"%s\" is satisfiable. \n", name);    
        		for(int i = 0; i < totalNumOfSymbols; i++) {
        		stringSolution[i*4] = getPropositionFromIndex(i);
        		stringSolution[i*4+1] = '=';
        		stringSolution[i*4+2] = solutionvector[i];
        		stringSolution[i*4+3] = ',';
        		fprintf(fpout,"%c%c%d%c",stringSolution[i*4],stringSolution[i*4+1],stringSolution[i*4+2],stringSolution[i*4+3] );
        		}
        		fprintf(fpout,"\n");

        	} else {
        	fprintf(fpout, "\"%s\" is not satisfiable\n", name);    

        	}
        
        }
        else {
        fprintf(fpout, "\"%s\" not a formula.\n", name);
    	}

        deleteTree(&t);
        lineCounter++;
    }

    
    fclose(fp);
    fclose(fpout);
    free(name);
    
    return(0);
}
