// src/data_structures/include/hashtable.h

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "list.h"

/// \brief The size of the hash table.
# define HASH_TABLE_SIZE 65011

/// \brief The maximum size of a key in the hash table.
# define MAX_KEY_SIZE 256

/**
 * \typedef hashtable
 * \brief Type representing a hash table.
 *
 * The hash table is implemented as an array of linked lists (`list`).
 * Each entry in the array corresponds to a linked list that handles collisions.
 */
typedef list hashtable[HASH_TABLE_SIZE];

/**
 * \struct hash_location
 * \brief Structure representing the location of an entry in the hash table.
 *
 * This structure stores the key and the index in the hash table where the
 * entry is located.
 */
typedef struct {
  char *key; /**< The key associated with the entry. */
  int index; /**< The index in the hash table where the entry is located. */
} hash_location;

/**
 * \struct hash_entry
 * \brief Structure representing an entry in the hash table.
 *
 * This structure stores a key-value pair. The value is stored as a void pointer
 * to allow the storage of any type.
 */
typedef struct {
  char *key; /**< The key for the entry. */
  void *value; /**< Pointer to the value associated with the key. */
} hash_entry;

/**
 * \brief Hash function to generate a unique index for a given key.
 *
 * This function computes a hash value for a given key using the djb2 algorithm.
 *
 * \param key The key for which the hash is to be generated.
 * \return The computed hash value as an unsigned long integer.
 */
unsigned long int hash(char *);

/**
 * \brief Gets the index in the hash table for a given key.
 *
 * This function computes the hash of a key and returns the index
 * where the key-value pair should be stored in the hash table.
 *
 * \param key The key for which the index is to be found.
 * \return The index in the hash table corresponding to the key.
 */
int get_hashtable_index(char *);

/**
 * \brief Retrieves the value associated with a key in the hash table.
 *
 * This function searches the hash table for the specified key and returns
 * the value associated with it.
 *
 * \param ht The hash table in which to search for the key.
 * \param hl A `hash_location` structure representing the key and its index.
 * \return A pointer to the value associated with the key, or NULL if not found.
 */
void * get_value(hashtable ht, hash_location hl);

/**
 * \brief Creates a new entry in the hash table.
 *
 * This function creates a new key-value pair and stores it in the hash table.
 * If the key already exists, the new entry is added to the list for that index.
 *
 * \param ht The hash table in which to store the entry.
 * \param key The key to be added.
 * \param value The value to be associated with the key.
 * \return A `hash_location` structure representing the location of the new entry.
 */
hash_location create_entry(hashtable ht, char * key, void *value);

/**
 * \brief Cleans up a hash entry.
 *
 * This function frees the memory associated with a hash entry, including
 * the key and value.
 *
 * \param he A pointer to the `hash_entry` to be cleaned up.
 */
void cleanup_hash_entry(hash_entry *he);

/**
 * \brief Cleans up the entire hash table.
 *
 * This function frees all the memory associated with the hash table,
 * including all entries and their keys and values.
 *
 * \param ht The hash table to be cleaned up.
 */
void cleanup_hashtable(hashtable ht);

#endif //HASHTABLE_H
