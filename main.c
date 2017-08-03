//
//  Created by Hamidulah Doust on 28.12.16.
//  Copyright © 2016 Hamidulah Doust. All rights reserved.
//
#include "constants.h"
#include "parser.h"
#include "completeTableau.h"
#include "createSolutionFromTableau.h"
#include "helperMethods.h"

// an upper bound for the amount of characters a single formula can have
// edit this variable if you use formulas with more characters than 50
int MAXIMUM_SIZE_OF_FORMULA=50;

void printCurrentTable() {
  int total = getTotalNumberOfPropositions();
  char values [total];
  for(int i = 0; i < total; i++ ) {
    values[i] = (char) getPropositionFromIndex(i);
    printf("Value with index %d is %c \n",i, values[i] );
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

int main(int argc, char * argv[]) {
  /*input a string and check if its a propositional formula */
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
    // parse checks if it is syntactically correct and stores all propositions
    if (parse(name)!=0) {
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
    } else {
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
