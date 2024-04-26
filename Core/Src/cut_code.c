#include "cut_code.h"

/* CUT CODES */
uint32_t num_codes = 2;



/* (x,y) */
Cut_Code codes[] = {
    { .length = 4, .name = "ETHN", .circumference = 3.14159265359, .code = (double[][2])  {
	    { 0.0,0.0},
	    { 1, 1},
	    { 2,-1},
	    { 3.14, 0},
    } },
    { .length = 2, .name = " GAY", .circumference = 3.14159265359, .code = (double[][2])  {
          { 0.0,0.0},
          { 3.14, -1},
        } },
};

