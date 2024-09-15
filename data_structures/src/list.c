/**
* \file list.c
 * \brief Implementation of functions for managing linked lists.
 *
 * This file contains the implementation of functions related to manipulating
 * linked lists, including adding and removing elements, as well as memory cleanup.
 *
 * \author Sylvain Labopin
 * \date 2024-09-09
 */

#include "data_structures/list.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "logger/logger.h"

list push_list(list *l, void *e) {
  list new = malloc(sizeof(cons));
  if (new) {
    new->car = e;
    new->cdr = *l;
    *l = new;
    return *l;
  } else {
    fprintf(stderr, "Failed to allocate memory for new node\n");
    log_error("Failed to allocate memory for new node\n");
    exit(EXIT_FAILURE);
  }
}

list to_list(int length, ...) {
  list l = NULL;
  va_list args;
  va_start(args, length);
  for (int i = 0; i < length; i++) push_list(&l, va_arg(args, void *));
  va_end(args);
  return l;
}


void reverse(list *lp) {
  if (!(*lp)) return;
  list prev = NULL;
  list current = *lp;
  list next = NULL;
  while (current) {
    next = current->cdr;
    current->cdr = prev;
    prev = current;
    current = next;
  }
  *lp = prev;
}

void cleanup_list(list * lp) {
  while (* lp) {
    list mem = (* lp);
    (* lp) = (* lp)->cdr;
    free(mem->car);
    mem->car = NULL;
    free(mem);
    mem = NULL;
  }
}