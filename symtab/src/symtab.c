/**
* \file symtab.c
 * \brief Implementation of functions for managing the symbol table.
 *
 * This file contains the implementation of the functions declared in
 * `symtab.h`. It provides functionalities to declare symbols, check if they
 * are already declared, and evaluate their values. The symbol table is
 * implemented as a hash table. In this implementation, each 'symbol' is
 * represented as a 'hash_location', which is defined in `hashtable.h`.
 * This allows for efficient mapping and retrieval operations typical of
 * hash tables.
 *
 * \author Sylvain Labopin
 * \date 2024-09-10
 */

#include "symtab/symtab.h"
#include "data_structures/list.h"

//#include <string.h>

#include "logger/logger.h"

symbol_table SYMBOL_TABLE = {NULL};
symbol_table_stack SYMBOL_TABLE_STACK = {&SYMBOL_TABLE, NULL};

typed_data eval(symbol s) {
  return * (typed_data *) get_value(SYMBOL_TABLE, s);
}

char is_declared(char *key) {
  hash_location hl = {key, get_hashtable_index(key)};
  return get_value(SYMBOL_TABLE, hl) != NULL;
}

symbol declare(char *key, typed_data *value) {
    if (is_declared(key)) {
      fprintf(stderr, "Attempt to redefine symbol %s\n", key);
      log_error("Attempt to redefine symbol %s\n", key);
      exit(EXIT_FAILURE);
    }
    return create_entry(SYMBOL_TABLE, key, value);
}
