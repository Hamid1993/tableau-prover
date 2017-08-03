//
//  createSolutionFromTableau.h
//  
//
//  Created by Hamid Doust on 02.08.17.
//
//
#ifndef createSolutionFromTableau_h
#define createSolutionFromTableau_h
#include "constants.h"
#include "helperMethods.h"
#include "parser.h"

int * createSolutionVectorForLeave(struct tableau* leave);
int * createSolutionVectorForRoot(struct tableau * t);
int getTotalNumberOfPropositions();

#endif /* createSolutionFromTableau_h */
