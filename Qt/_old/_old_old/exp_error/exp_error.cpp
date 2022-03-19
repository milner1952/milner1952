#include "exp_error.h"
 void EXP_ERROR::ErrorSetText(const char* lib)
 {
   text_out = lib;
   text_out  += "\r\n";
   text_out += info;
 }

