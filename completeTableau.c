//
//  completeTableau.c
//  
//
//  Created by Hamid Doust on 02.08.17.
//
//

#include "completeTableau.h"

// only gets invoked if the table is initialzied with a correct formula
void complete(struct tableau *t) {
    
    // use depth first search to complete the tableau t
    depthFirstSearch(t);
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











