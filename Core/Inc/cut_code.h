#ifndef CUT_CODE_H
#define CUT_CODE_H

#include <stdint.h>
#include "main.h"

extern double circumference;

extern uint32_t num_instructions;
extern double cut_instructions[][2];

typedef struct {
  double circumference;
  uint32_t length;
  char name[4];
  double (*code)[2];
} Cut_Code;

#endif // CUT_CODE_H
