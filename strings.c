#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "strings.h"
#include "target.h"

#if windows
char *strtok_r(char *str, const char *delim, char **save)
{
  char *res, *last;

  if( !save )
    return strtok(str, delim);
  if( !str && !(str = *save) )
    return NULL;
  last = str + strlen(str);
  if( (*save = res = strtok(str, delim)) )
    {
      *save += strlen(res);
      if( *save < last )
        (*save)++;
      else
        *save = NULL;
    }
  return res;
}
#endif

Definition** StringList_definitions = NULL;
int StringList_num_definitions = 0;

void StringList_init() {
  if(StringList_definitions == NULL) {
    StringList_definitions = (Definition**) calloc(1, sizeof(Definition*));
  }
}

Definition *Definition_new(const char* name, const char* value) {
  Definition* self = (Definition*) calloc(1, sizeof(Definition));

  self->name = (char*) calloc(strlen(name)+1, sizeof(char));
  strncpy(self->name, name, strlen(name)+1);

  self->value = (char*) calloc(strlen(value)+1, sizeof(char));
  strncpy(self->value, value, strlen(value)+1);

  return self;
}

bool StringList_add_definition(const char* name, const char* value) {

  if(StringList_has_definition(name)) {
    return false;
  }
  Definition* definition = Definition_new(name, value);

  StringList_definitions =
    (Definition**) realloc(StringList_definitions,
                           (StringList_num_definitions+1) * sizeof(Definition*));

  StringList_definitions[StringList_num_definitions] = definition;

  StringList_num_definitions++;
  return true;
}

bool StringList_has_definition(const char* name) {
  return StringList_get_definition(name) != NULL;
}

Definition* StringList_get_definition(const char* name) {
  Definition* definition;
  for(int i=0; i<StringList_num_definitions; i++) {
    definition = StringList_definitions[i];
    if(strcmp(definition->name, name) == 0) {
      return definition;
    }
  }
  return NULL;
}

StringList *StringList_new(void) {
  StringList_init();
  
  StringList *stringlist = (StringList*) calloc(1, sizeof(StringList));
  stringlist->size = 0;
  stringlist->strings = (char**) NULL;  
  return stringlist;
}

void StringList_append(StringList *self, const char *string, const char* delim) {  

  if(StringList_has_definition(string)) {
    StringList_append_tokenized(self, StringList_get_definition(string)->value, delim);
  }
  else {
    self->strings = (char**) realloc(self->strings, (self->size+1) * sizeof(char *));
    self->strings[self->size] = calloc(strlen(string)+1, sizeof(char));
    strncpy(self->strings[self->size], string, strlen(string));
    self->size++;
  }
}

void StringList_append_tokenized(StringList *self, const char* input, const char *delim) {
  char *substring;
  char *saveptr;
  
  char *string = (char*) calloc(strlen(input)+1, sizeof(char));
  strncpy(string, input, strlen(input));
  
  if((substring = strtok_r(string, delim, &saveptr)) != NULL) {
    StringList_append(self, substring, delim);
  
    while((substring = strtok_r(NULL, delim, &saveptr)) != NULL) {
      StringList_append(self, substring, delim);
    } 
  }
  free(string);
}

char* StringList_get(StringList *self, int index) {
  if(index >= 0 && index < self->size) {
    return self->strings[index];
  }
  return NULL;
}

char* StringList_last(StringList *self) {
  return self->strings[self->size-1];
}

void StringList_remove_last(StringList *self) {
  free(self->strings[--self->size]);
}

void StringList_free(StringList *self) {
  for(int i=0; i<self->size; i++) {
    free(self->strings[i]);
  }
  free(self->strings);
  free(self);
}
