#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "range.h"

Range* Range_new(int start, int end) {

  Range* self = (Range*) calloc(1, sizeof(Range));
  self->min = 0;
  self->max = 0x10000;
  self->start = start;
  self->end = end;
  return self;
}

Range* Range_new_from_int(int range) {

  int start = ((unsigned int) range) >> 16;
  int end = range & 0xffff;
  if(end == 0) end = 0x10000;

  return Range_new(start, end);
}

Range* Range_parse(char *str) {

  Range* self = Range_new(0, 0x10000);

  if(strlen(str) == 0) {
    return self;
  }
  
  char *start = str;
  char *end = strstr(str, "-");

  if(end != NULL) {
    end++;
  }
  
  while(isspace((int)start[0])) start++;
  
  if(strlen(start) > 0 && start[0] != '-') {
    self->start = (int) strtol(start, NULL, 0);
  }

  if(end == NULL) {
    self->end = -1;
  }
  else {
    while(isspace((int)end[0])) end++;
    
    if(strlen(end) > 0) {
      self->end = (int) strtol(end, NULL, 0);
    }
  }
  return self;
}

bool Range_valid(Range* self) {
  return
    self->start >= self->min &&
    self->start <= self->max &&
    self->end >= self->min &&
    self->end <= self->max &&
    self->end >= self->start;
}

void Range_print(Range* self) {
  printf("$%04X-$%04X\n", self->start, self->end);
}

int Range_size(Range* self) {
  return self->end - self->start;
}

bool Range_equals(Range* self, Range* range) {
  return self->start == range->start && self->end == range->end;
}

bool Range_ends(Range* self) {
  return self->end != -1;
}

void Range_move(Range* self, int amount) {

  if(amount == 0) return;
  
  if(amount > 0) {
    if(self->end + amount > self->max) {
      amount -= (self->end + amount) - self->max;
    }
  }

  if(amount < 0) {
    if(self->start + amount < self->min) {
      amount -= (self->start + amount);
    }
  }
  
  self->start += amount;
  self->end += amount;
}

bool Range_inside(Range* self, Range* range) {
  if(Range_equals(self, range)) {
    return true;    
  }
  return self->start >= range->start && self->end <= range->end;
}

bool Range_outside(Range* self, Range* range) {
  return !Range_inside(self, range);
}

bool Range_overlaps(Range* self, Range* range) {

  if(Range_equals(self, range)) {
    return true;
  }

  return
    (self->start >= range->start && self->start <= range->end) ||
    (self->end >= range->start && self->end <= range->end) ||  
    (range->start >= self->start && range->start <= self->end) ||
    (range->end >= self->start && range->end <= self->end);  
}

void Range_free(Range* self) {
  free(self);
}
