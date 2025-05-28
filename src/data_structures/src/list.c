// src/data_structures/src/list.c

#include <stddef.h>
#include <stdlib.h>

#include "logger.h"

#include "list.h"

list list_push(list l, void * e) {
    if (!e) return NULL;
    list ret = list_malloc(sizeof(cons));
    if (!ret) return (list) NULL;
    ret->car = e;
    ret->cdr = l;
    return ret;
}
