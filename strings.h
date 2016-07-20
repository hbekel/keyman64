#ifndef STRINGS_H
#define STRINGS_H

#include <stdbool.h>

typedef struct {
  int size;
  char **strings;
} StringList;

typedef struct {
  char* name;
  char* value;
} Definition;

void StringList_init(void);
bool StringList_add_definition(const char* name, const char* value);
bool StringList_has_definition(const char* name);
Definition* StringList_get_definition(const char* name);

StringList *StringList_new(void);
void StringList_append(StringList *self, const char *string, const char *delim);
void StringList_append_tokenized(StringList *self, const char* input, const char *delim);
char* StringList_get(StringList *self, int index);
char* StringList_last(StringList *self);
void StringList_remove_last(StringList *self);
void StringList_free(StringList *self);

#endif // STRINGS_H
