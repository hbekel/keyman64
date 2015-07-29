#ifndef STRINGS_H
#define STRINGS_H

typedef struct {
  int size;
  char **strings;
} StringList;

StringList *StringList_new(void);
void StringList_append(StringList *self, char *string);
void StringList_append_tokenized(StringList *self, const char* input, const char *delim);
char* StringList_get(StringList *self, int index);
char* StringList_last(StringList *self);
void StringList_remove_last(StringList *self);
void StringList_free(StringList *self);

#endif // STRINGS_H
