#ifndef COLUMS_COREUTILS_COMMON_H
#define COLUMS_COREUTILS_COMMON_H

#include <stdio.h>

extern int Flags;

#define Destroy(x) (x ? free(x) : 0)

char *CopyStrLen(char *Dest, const char *Src, size_t len);
char *CopyStr(char *Dest, const char *Src);

char *MCatStr(char *Dest, const char *Str1,  ...);
char *MCopyStr(char *Dest, const char *Str1,  ...);


inline char *AddCharToBuffer(char *Dest, size_t DestLen, char Char);

void ParseCommandValue(int argc, char *argv[], int pos, int Flag, char **String);
void GetMinMaxFields(const char *FieldSpec, int *MinField, int *MaxField);
char *FILEReadLine(char *RetStr, FILE *f, char Term);

void StripTrailingWhitespace(char *str);
void StripCRLF(char *str);
char *FormatStr(char *InBuff, const char *FmtStr, ...);



#endif
