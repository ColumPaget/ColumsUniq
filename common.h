#ifndef COLUMS_UNIQ_COMMON_H
#define COLUMS_UNIQ_COMMON_H

#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

extern int Flags;

#define FALSE 0
#define TRUE 1

#define StrLen(str) ((str) ? strlen(str) : 0)
#define Destroy(x) ((x) ? free(x) : 0)

char *CopyStrLen(char *Dest, const char *Src, size_t len);
char *CopyStr(char *Dest, const char *Src);

char *CatStr(char *Dest, const char *Src);
char *CatStrLen(char *Dest, const char *Src, size_t len);
char *MCatStr(char *Dest, const char *Str1,  ...);
char *MCopyStr(char *Dest, const char *Str1,  ...);


char *AddCharToBuffer(char *Dest, size_t DestLen, char Char);

void ParseCommandValue(int argc, char *argv[], int pos, int Flag, char **String);
void GetMinMaxFields(const char *FieldSpec, int *MinField, int *MaxField);
char *FILEReadLine(char *RetStr, FILE *f, char Term);

void StripTrailingWhitespace(char *str);
void StripCRLF(char *str);
char *FormatStr(char *InBuff, const char *FmtStr, ...);



#endif
