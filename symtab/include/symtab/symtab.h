/**
* \file symtab.h
 * \brief Header file for the symbol table operations.
 *
 * This file contains the declarations of functions and structures for managing
 * the symbol table. The symbol table is a hash table that stores symbols and
 * their associated values.
 * It includes functionality to declare new symbols, check if a symbol is
 * already declared, and evaluate the value of a symbol.
 *
 * \author Sylvain Labopin
 * \date 2024-09-10
 */

#ifndef SYMTAB_H
#define SYMTAB_H

#include "data_structures/hashtable.h"

/**
 * \typedef symbol_table
 * \brief Type representing the symbol table.
 *
 * The symbol table is implemented as a hash table that stores symbols (keys)
 * and their associated values.
 */
typedef hashtable symbol_table;

/**
 * \brief Current active symbol table.
 *
 * SYMBOL_TABLE points to the symbol table at the top of the symbol table
 * stack, which corresponds to the most recent call context. This table
 * contains the symbols relevant to the current scope, enabling dynamic
 * management of symbol visibility and lifetimes as functions are
 * entered and exited.
 */
extern symbol_table SYMBOL_TABLE;

/**
 * \brief Type definition for the symbol table stack.
 *
 * Defines a list type structure used to manage multiple levels of symbol
 * tables. Each level corresponds to a different call context and contains
 * symbols specific to that context. This enables scoped management of
 * symbols across various execution contexts.
 */
typedef list symbol_table_stack;

/**
 * \brief Global stack for managing symbol tables.
 *
 * This stack manages different levels of symbol tables, each corresponding
 * to a unique call context for scoped symbol management. Symbols within
 * each table are specific to their respective context, supporting nested
 * scoping such as in function calls or conditional blocks.
 */
extern symbol_table_stack SYMBOL_TABLE_STACK;

/**
 * \typedef symbol
 * \brief Structure representing the information needed to access an entry in the
 *        hash table.
 *
 * A symbol, defined as `hash_location`, structures the necessary information to
 * access a specific entry in the `SYMBOL_TABLE`. This entry is of type `hash_entry`
 * and consists of a key-value pair. This typedef uses the structure defined in
 * `hashtable.h` to facilitate efficient access and management of symbols in the
 * hash table.
 */
typedef hash_location symbol;

/**
 * \enum data_type
 * \brief Enumeration of constants for data types associated with symbols.
 *
 * This enum defines constants that represent the types of values associated
 * with symbols in the symbol table. It is used as the type for the first field
 * in the `typed_data` structure, indicating the data type of a symbol's value.
 */
typedef enum {
    TYPE_INT, /**< Integer type. */
    TYPE_STRING, /**< String type, can be code snippet */
    TYPE_SYMBOL, /**< Symbol type (pointer to another symbol). */
    //TYPE_FUNCTION, /**< Placeholder for a function type. */
} data_type;

/**
 * \struct typed_data
 * \brief Structure representing a value associated with a symbol.
 *
 * This structure contains a `type` indicating the kind of data and the actual
 * data value, which varies depending on the type. It can be an integer, a string,
 * or a pointer to another symbol. This allows for storing different types of data
 * in the symbol table under a unified interface.
 */
typedef struct {
    data_type type; /**< Type of the data (e.g., int, string, symbol). */
    union {
        int int_value; /**< Integer value. */
        char *string_value; /**< String value. */
        symbol *symbol_value; /**< Pointer to another symbol. */
        //function *function_value; /**< Placeholder for function value. */
    } data;
} typed_data;

/**
 * \brief Evaluates the value of a symbol.
 *
 * This function retrieves the value associated with a given symbol from the
 * symbol table.
 *
 * \param s The symbol to be evaluated.
 * \return The `typed_data` structure containing the type and value of the symbol.
 */
typed_data eval(symbol s);

/**
 * \brief Checks if a symbol is already declared in the symbol table.
 *
 * This function checks if the given key (symbol name) has already been
 * declared in the symbol table.
 *
 * \param key The name of the symbol to check.
 * \return 1 if the symbol is declared, 0 otherwise.
 */
char is_declared(char *key);

/**
 * \brief Declares a new symbol in the symbol table and returns its location.
 *
 * This function checks for prior declaration of the symbol. If found, it logs an
 * error, outputs a message, and terminates the program. If the symbol is not
 * previously declared, it creates a new hash entry in the hashtable and returns
 * the location of this entry. The return type, `hash_location`, is typedefed as
 * `symbol`.
 *
 * \param key The name of the symbol to declare.
 * \param value A pointer to the value to be associated with the symbol, which can
 *              be NULL if the symbol does not need immediate resolution.
 * \return A `symbol` (typedef of `hash_location`) indicating the insertion point.
 * \throws Exits with `EXIT_FAILURE` on redeclaration or memory allocation failure.
 */

symbol declare(char *, typed_data *);

#endif //SYMTAB_H
