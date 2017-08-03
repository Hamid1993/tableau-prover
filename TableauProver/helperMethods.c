//
//  helperMethods.c
//  
//
//  Created by Hamid Doust on 02.08.17.
//
//

#include "helperMethods.h"

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
    counter++;
    // value doesnt get used in this case, just to avoid compiler warning
    return 0;
  }
}

struct symbolNode* createSymbolNode(char symbol, int value) {
  // doesnt exist yet, so add new symbol
  struct symbolNode * newSymbol = malloc(sizeof(struct symbolNode));
  newSymbol->symbol = symbol;
  newSymbol->value = value;
  newSymbol->next = NULL;
  return newSymbol;
}


