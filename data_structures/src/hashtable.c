/**
 * \file hashtable.c
 * \brief Implementation of functions for managing a hash table.
 *
 * This file contains the implementation of the functions declared in
 * `hashtable.h` for creating, accessing, and cleaning up a hash table.
 *
 * \author Sylvain Labopin
 * \date 2024-09-10
 */

#include "data_structures/hashtable.h"

#include <stdlib.h>
#include <string.h>

#include "logger/logger.h"

unsigned long int hash(char * key) {
  unsigned long int hash = 5381 ;
  while (* key) {
    int c = * key ;
    hash = ((hash << 5) + hash) + c ;
    key++ ;
  }
  return hash ;
}

int get_hashtable_index(char * key) {
  return hash(key) % HASH_TABLE_SIZE;
}

hash_location create_entry(hashtable hashtable, char * key, void *value) {
  hash_location location;
  location.key = strdup(key);
  location.index = get_hashtable_index(key);
  list list_of_entries = malloc(sizeof(cons));
  if (!list_of_entries) {
    fprintf(stderr, "Failed to allocate memory for a list of hash entries\n");
    log_error("Failed to allocate memory for a list of hash entries\n");
    exit(EXIT_FAILURE);
  }
  hash_entry *new_entry = malloc(sizeof(hash_entry));
  if (!new_entry) {
    fprintf(stderr, "Failed to allocate memory for a new hash entry\n");
    log_error("Failed to allocate memory for a new hash entry\n");
    exit(EXIT_FAILURE);
  }
  new_entry->key = location.key;
  new_entry->value = value;
  list_of_entries->car = new_entry;
  list_of_entries->cdr = hashtable[location.index];
  hashtable[location.index] = list_of_entries;
  return location;
}

void * get_value(hashtable hashtable, hash_location hl) {
  list list_of_entries = hashtable[hl.index];
  while (list_of_entries) {
    if (strcmp(hl.key, (* ((hash_entry *) list_of_entries->car)).key) == 0)
      return ((hash_entry *) list_of_entries->car)->value;
    list_of_entries = list_of_entries->cdr;
  }
  return NULL;
}

void cleanup_hash_entry(hash_entry *hep) {
  free(hep->key);
  hep->key = NULL;
  free(hep->value);
  hep->value = NULL;
}

void cleanup_hashtable(hashtable hashtable) {
  list l;
  for (int i = 0; i < HASH_TABLE_SIZE; i++) {
    l = hashtable[i];
    while (l && l->car) {
      cleanup_hash_entry((hash_entry *) l->car);
      l = l->cdr;
    }
    cleanup_list(&hashtable[i]);
  }
}
