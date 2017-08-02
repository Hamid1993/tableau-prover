//
//  createSolutionFromTableau.c
//  
//
//  Created by Hamid Doust on 02.08.17.
//
//

#include "createSolutionFromTableau.h"
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

// returns the total number of propositions seen so far
int getTotalNumberOfPropositions() {
    return returnSymbolValue('a', -2);
}
