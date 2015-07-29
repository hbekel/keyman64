#ifndef RANGE_H
#define RANGE_H

typedef struct {
  int start;
  int end;
  int min;
  int max;
} Range;

Range* Range_new(int start, int end);
Range* Range_new_from_int(int range); 
Range* Range_parse(char *str);
bool Range_equals(Range* self, Range* range);
bool Range_valid(Range* self);
int Range_size(Range* self);
bool Range_ends(Range *self);
void Range_move(Range* self, int amount);
bool Range_inside(Range* self, Range* range);
bool Range_outside(Range* self, Range* range);
bool Range_overlaps(Range* self, Range* range);
void Range_print(Range* self);
void Range_free(Range* self);

#endif // RANGE_H
