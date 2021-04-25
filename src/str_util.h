#pragma once
#ifndef STR_UTIL_H
#define STR_UTIL_H

#include "global.h"

// returns the rest of the string after the identifier or NULL if the identifier isn't included
// taken from: https://stackoverflow.com/questions/44539255/finding-last-occurence-of-string-in-c
char* str_find_last_of(char* str, char* identifier);

// taken from: https://www.delftstack.com/howto/c/truncate-string-in-c/
// cut off "pos" amount of chars
// if "pos" id negative it will cut from the back
char* str_trunc(char* str, int pos);

#endif
