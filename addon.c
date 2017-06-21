
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


 struct symbolNode{
    char symbol;
    int value;
    struct symbolNode * next;
};


struct tableau {
    char *root;
    struct  tableau *left;
    struct tableau *right;
    struct tableau *parent;
    
    // determines if the node in the tableau was fully processed
    // 1 means it was fully processed, 0 means it wasnt
    int finished ;
}*tab, *node, *node1, *kid, *pa;




int isFinished(char * formula) {
  
    return 0;
}



struct symbolNode* createSymbolNode(char symbol, int value) {

      // doesnt exist yet, so add new symbol
    struct symbolNode * newSymbol = malloc(sizeof(struct symbolNode));
    newSymbol->symbol = symbol;
    newSymbol->value = value;
    newSymbol->next = NULL; 
    return newSymbol;
}

// returns the value of symbol
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

// returns 1 if the next symbol is a proposition, otherwise 0
int isProposition(char symbol) {
    static char specialSymbols [] = {'^','v','<','>','(',')','-'};

    for (int i = 0; i < sizeof(specialSymbols)/sizeof(char); i++) {
        if (specialSymbols[i] == symbol) {
            return 0;
        }
    }
    return 1;
}






// creates a possible solution for a proposition in form of a tableau, which represents the leave
// if the first entry of the array contains -1 then the solution is invalid, otherwise valid
int * createSolutionVectorForLeave(struct tableau* leave) {
    
    // use two boolean arrays, one tracks the occurences of propositions, the other one of negated propositions where 1 means "occured" and 0 means "not occured"
    // -2 indicates that we want to have the number of symbols stored so far
    int numberOfSymbols = returnSymbolValue('a',-2);
    int  propositions [numberOfSymbols];
    int negatedPropositions [numberOfSymbols];

    int * solutionvector  = (int *) malloc(sizeof(int)*numberOfSymbols);

    
    struct tableau * current = leave;
    
    do {
    
        // check if the current formula is a proposition or a negated proposition
        if(isFinished(current->root)) {
            
            if(current->root[0] == '-' ) {
                
                int index = returnSymbolValue(current->root[1], -1);
                // mark that this negated proposition has occured
                // it doesnt matter how often a proposition/ negated proposition occurs
                negatedPropositions[index] = 1;
                
            } else {
                int index = returnSymbolValue(current->root[0], -1);
                // mark that this proposition has occured
                propositions[index] = 1;
            }
        }
        
         // go one to the top
        current = current->parent;
            
        
    }while (current != NULL);
    
  
    int errorValue = 0;
    
    // check if the array only contains valid values
    for(int i = 0; i < 3; i++) {
        
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

    int totalNumOfSymbols = returnSymbolValue('a', -2);
    int * leftSolutionVector  = malloc(sizeof(int)*totalNumOfSymbols);
    int * rightSolutionVector = malloc(sizeof(int)*totalNumOfSymbols);



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
    if (leftSolutionVector[0]!= -1) {
        // free other memory which is not needed
        free (rightSolutionVector);
        return leftSolutionVector;
    } else {
        free(leftSolutionVector);
        return rightSolutionVector;
    }
    
    
}




int main () {


int myval = returnSymbolValue('a',-1);
int mysecondval = returnSymbolValue('b', -1);
int thirdval = returnSymbolValue('a', -1);
int fourthVal = returnSymbolValue('b',-1);
int totalnum = returnSymbolValue('a', -2);
int valueOfB = returnSymbolValue('z',1);



printf("First value: %d \n",myval);
printf("Second value: %d \n",mysecondval);
printf("Third value: %d \n",thirdval);
printf("Fourth value: %d \n",fourthVal);

printf("Total number of symbols:: %d \n",totalnum);

int firstPropo = isProposition('z');
int secondPropo = isProposition('v');


printf("firstPropo value: %d \n",firstPropo);
printf("secondPropo value: %d \n",secondPropo);


char c = (char) valueOfB;
printf("Value of 1: %c\n",c );

}


