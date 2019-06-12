#include "common.h"
#include <ctype.h>

#define FLAG_FIRST 1
#define FLAG_LAST 2
#define FLAG_UNIQ 4
#define FLAG_DELIM 8
#define FLAG_NOCASE 32
#define FLAG_LENGTH 64
#define FLAG_DUPLICATES 128
#define FLAG_COUNTS 256
#define FLAG_COUNTS_NOSPACE 512
#define FLAG_COUNTS_UPDATE 1024
#define FLAG_FIELDS 2048
#define FLAG_FIELDSKIP 4096
#define FLAG_HIGHLIGHT_KEY 8192
#define FLAG_SPACED 16384
#define FLAG_SKIP 32768
#define FLAG_ZERO_TERM 65536

#define HL_NORM "\x1b[0m"
#define HL_BOLD "\x1b[01m"
#define HL_UNDER "\x1b[03m"
#define HL_INVERSE "\x1b[7m"


//Set a limit of line length that we will consider, so we won't slurp gigabites into memory
#define FULL_LINE 4096

#define COMPARE_FLAGS (FLAG_NOCASE | FLAG_LENGTH)
#define OUTPUT_FLAGS (FLAG_FIRST | FLAG_LAST | FLAG_COUNTS | FLAG_COUNTS_NOSPACE | FLAG_UNIQ | FLAG_DUPLICATES | FLAG_HIGHLIGHT_KEY)


//Flags is defined in common.c
//int Flags=0;

int CompareLength=FULL_LINE;
int CompareStart=0;
char *Delim=NULL, *InPath=NULL, *OutPath=NULL, *FieldSpecifier=NULL, *Highlight=NULL;
char *Version="1.0";
char *LineTerminator=NULL;
FILE *In=NULL, *Out=NULL;

#define OPT_NAME  1
#define OPT_SHORT 2
#define OPT_LONG  3

void OutputVersion()
{
printf("uniq (Colums coreutils) %s\n",Version);
printf("Copyright (C) 2015 Colum Paget\n");
printf("License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n");
printf("This is free software: you are free to change and redistribute it.\n");
printf("There is NO WARRANTY, to the extent permitted by law.\n");
printf("\nWritten by Colum Paget (colums.projects@gmail.com)\n");
exit(0);
}



void OutputHelp()
{
printf("Usage: uniq [OPTION]... [INPUT [OUTPUT]]\n");
printf("Filter adjacent matching lines from INPUT (or standard input),\n");
printf("writing to OUTPUT (or standard output).\n");
printf("\n");
printf("With no options, matching lines are merged to the first occurrence.\n");
printf("\n");
printf("Mandatory arguments to long options are mandatory for short options too.\n");
printf("  -c, --count           prefix lines by the number of occurrences\n");
printf("  -C, --increment       prefix lines by the number of occurrences, handle input lines that already contain counts\n");
printf("  -d, --repeated        only print duplicate lines\n");
printf("  -D, --all-repeated[=delimit-method]  print all duplicate lines\n");
printf("                        delimit-method={none(default),prepend,separate}\n");
printf("                        Delimiting is done with blank lines\n");
printf("  -f, --skip-fields=N   avoid comparing the first N fields\n");
printf("  -i, --ignore-case     ignore differences in case when comparing\n");
printf("  -n,                   suppress leading spaces of counts\n");
printf("  -s, --skip-chars=N    avoid comparing the first N characters\n");
printf("  -t, --delims=[delimters]  list of characters treated as field delimiters\n");
printf("  -F, --field=N         compare only this field on each line\n");
printf("  -u, --unique          only print unique lines\n");
printf("  -z, --zero-terminated  end lines with 0 byte, not newline. This applies to both input and output. Output termination can be overridden with --endline\n");
printf("  -L, --endline=[string] Terminate lines of output with the given string\n");
printf("  -w, --check-chars=N   compare no more than N characters in lines\n");
printf("  -1, --first           When collating lines, display the FIRST line\n");
printf("  -Z, --last            When collating lines, display the LAST line\n");
printf("  -?, --help     display this help and exit\n");
printf("      --version  output version information and exit\n");
printf("\n");
printf("Fields are broken up by delimiter characters specified by the -t or --delims options. The default delimters are tab and space.\n");
printf("Some options cause uniq to consider strings with different case as identical, or to consider substrings, rather than the entire string, so that two strings can be considered identical because their selected characters match. In these cases the -1 and -Z flags can be used to select the first or last occurance of a matching series.\n");
printf("\n");
printf("Note: 'uniq' does not detect repeated lines unless they are adjacent.\n");
printf("You may want to sort the input first, or use 'sort -u' without 'uniq'.\n");
printf("(-i, -s, -w, -f and -F can cause non identical lines to be collated together)\n");
printf("\n");
printf("Report bugs to colums.projects@gmail.com\n");
exit(0);
}







void SetHighlight(const char *Value)
{
if (strcmp(Value,"b")==0) Highlight=CopyStr(Highlight,HL_BOLD);
if (strcmp(Value,"u")==0) Highlight=CopyStr(Highlight,HL_UNDER);
if (strcmp(Value,"i")==0) Highlight=CopyStr(Highlight,HL_INVERSE);
}

void ParseCommandLine(int argc, char *argv[])
{
int i, State=OPT_NAME;
char *Tempstr=NULL, *ptr, *arg;

for (i=1; i < argc; i++)
{
	State=OPT_NAME;
	ptr=argv[i];
	while (*ptr=='-') 
	{
		State++;
		ptr++;
	}


	switch (State)
	{
		case OPT_NAME:
			if (! StrLen(InPath)) InPath=CopyStr(InPath, argv[i]);
			else if (! StrLen(OutPath)) OutPath=CopyStr(OutPath, argv[i]);
		break;


		case OPT_SHORT:
			while ((*ptr != '\0') && (! isspace(*ptr)))
			{
			switch (*ptr)
			{
				case 'c': Flags |= FLAG_COUNTS; break;
				case 'C': Flags |= FLAG_COUNTS | FLAG_COUNTS_UPDATE; break;
				case '1': Flags |= FLAG_FIRST; break;
				case 'Z': Flags |= FLAG_LAST; break;
				case 'H': 
					Flags |= FLAG_HIGHLIGHT_KEY; 
					ptr++;
					if (*ptr=='=') SetHighlight(ptr+1);
				break;
				case 'i': Flags |= FLAG_NOCASE; break;
				case 'f': 
					ParseCommandValue(argc, argv, ++i, FLAG_FIELDSKIP | FLAG_FIELDS, &FieldSpecifier); 
					FieldSpecifier=CatStr(FieldSpecifier,"-");
				break;
				case 'F': ParseCommandValue(argc, argv, ++i, FLAG_FIELDS | FLAG_LENGTH, &FieldSpecifier); break;
				case 't': ParseCommandValue(argc, argv, ++i, FLAG_DELIM, &Delim); break;
				case 'L': ParseCommandValue(argc, argv, ++i, 0, &LineTerminator); break;
				case 'n': Flags |= FLAG_COUNTS_NOSPACE; break;
				case 'u': Flags |= FLAG_UNIQ; break;
				case 'd': Flags |= FLAG_DUPLICATES; break;
				case 'S': Flags |= FLAG_SPACED; break;
				case 's':
					ParseCommandValue(argc, argv, ++i, 0, &Tempstr);
					CompareStart=atoi(Tempstr);
				break;
				case 'w': 
					ParseCommandValue(argc, argv, ++i, 0, &Tempstr);
					CompareLength=atoi(Tempstr);
				break;
				case 'z': Flags |= FLAG_ZERO_TERM; LineTerminator=CopyStr(LineTerminator,""); break;
				case '?': OutputHelp(); break;
			}
			ptr++;
			}
		break;



		case OPT_LONG:
			arg=strchr(ptr,'=');
			if (arg)
			{
				*arg='\0';
				arg++;
			}

			if (strcmp(ptr,"version")==0) OutputVersion();
			else if (strcmp(ptr,"help")==0) OutputHelp();
			else if (strcmp(ptr,"first")==0) Flags |= FLAG_FIRST;
			else if (strcmp(ptr,"last")==0) Flags |= FLAG_LAST;
			else if (strcmp(ptr,"count")==0) Flags |= FLAG_COUNTS;
			else if (strcmp(ptr,"increment")==0) Flags |= FLAG_COUNTS_UPDATE;
			else if (strcmp(ptr,"repeated")==0) Flags |= FLAG_DUPLICATES;
			else if (strcmp(ptr,"unique")==0) Flags |= FLAG_UNIQ;
			else if (strcmp(ptr,"ignore-case")==0) Flags |= FLAG_NOCASE;
			else if (strcmp(ptr,"endline")==0) LineTerminator=CopyStr(LineTerminator,arg);
			else if (strcmp(ptr,"check-chars")==0) 
			{
					if (StrLen(arg)) CompareStart=atoi(arg);
			}
			else if (strcmp(ptr,"skip-chars")==0) 
			{
					if (StrLen(arg)) CompareLength=atoi(arg);
			}
			else if (strcmp(ptr,"skip-fields")==0)
			{
				Flags |= FLAG_FIELDSKIP | FLAG_FIELDS;
				FieldSpecifier=MCopyStr(FieldSpecifier,arg,"-",NULL);
			}
		break;

		default:
		break;
	}
}


if ((! (Flags & FLAG_ZERO_TERM)) && (LineTerminator == NULL)) LineTerminator=CopyStr(LineTerminator,"\n");

Destroy(Tempstr);
}


int FieldStartEnd(char **start, int MinField, int MaxField)
{
char *ptr, *end=0;
int count;

	ptr=*start;
	for (count=0; count < CompareStart; count++)
	{
		if (*ptr=='\0') return(0);
		ptr++;
	}

	//save result into start
	*start=ptr;

	if (Flags & FLAG_FIELDS)
	{
		for (count=0; count < MinField; count++)
		{
		*start=ptr;
			while (*ptr !='\0')
			{
				if (strchr(Delim, *ptr)) break;
				ptr++;
			}
		end=ptr;
		if (*ptr=='\0') break;
		ptr++;
		}

		for (; count < MaxField; count++)
		{
		for (; *ptr !='\0'; ptr++)
		{
			if (strchr(Delim, *ptr)) break;
		}
		end=ptr;
		if (*ptr=='\0') break;
		ptr++;
	}
	}

	if (end < *start) return(FULL_LINE);
	return(end-*start);
}





char *HighlightKey(char *RetStr, char *Line, int MinField,  int MaxField)
{
char *ptr;
int len;

ptr=Line;
len=FieldStartEnd(&ptr, MinField, MaxField);

if (len > 0)
{
	RetStr=CatStrLen(RetStr,Line,ptr-Line);
	RetStr=CatStr(RetStr,Highlight);
	RetStr=CatStrLen(RetStr,ptr,len);
	if (len < FULL_LINE) RetStr=MCatStr(RetStr, HL_NORM, ptr+len,NULL);
	else RetStr=CatStr(RetStr, HL_NORM);
}

return(RetStr);
}



void OutputLine(char *Line)
{
int len, termlen;

	len=StrLen(Line);
	termlen=StrLen(LineTerminator);
	if ((Flags & FLAG_ZERO_TERM) && (termlen==0))
	{
		len++;
		fwrite(Line, 1, len, Out);
	}
	else 
	{
		fwrite(Line, 1, len, Out);
		fwrite(LineTerminator, 1, termlen, Out);
	}
}



void OutputLines(char *CurrLine, char *PrevLine, char *FirstLine, int MinField, int MaxField, int IdenticalCount)
{
char *Tempstr=NULL;

		switch (Flags & OUTPUT_FLAGS)
		{
			case 0:
			case FLAG_FIRST:
				if (FirstLine) Tempstr=CopyStr(Tempstr,FirstLine);
			break;

			case FLAG_HIGHLIGHT_KEY:
			case FLAG_FIRST | FLAG_HIGHLIGHT_KEY:
				if (FirstLine) Tempstr=HighlightKey(Tempstr, FirstLine, MinField,  MaxField);
			break;

			case FLAG_LAST:
				if (PrevLine) Tempstr=CopyStr(Tempstr,PrevLine);
			break;

			case FLAG_LAST | FLAG_HIGHLIGHT_KEY:
				if (PrevLine) Tempstr=HighlightKey(Tempstr, PrevLine, MinField,  MaxField);
			break;

			case FLAG_COUNTS:
			case FLAG_COUNTS | FLAG_LAST:
				if (IdenticalCount && PrevLine) Tempstr=FormatStr(Tempstr,"% 7d %s",IdenticalCount,PrevLine);
			break;

			case FLAG_COUNTS | FLAG_HIGHLIGHT_KEY:
			case FLAG_COUNTS | FLAG_LAST | FLAG_HIGHLIGHT_KEY:
				if (IdenticalCount && PrevLine) 
				{
					Tempstr=FormatStr(Tempstr,"% 7d ",IdenticalCount);
					Tempstr=HighlightKey(Tempstr, PrevLine, MinField,  MaxField);
				}
			break;

			case FLAG_COUNTS | FLAG_FIRST:
				if (IdenticalCount) Tempstr=FormatStr(Tempstr,"% 7d %s",IdenticalCount,FirstLine);
			break;

			case FLAG_COUNTS | FLAG_FIRST | FLAG_HIGHLIGHT_KEY:
				if (IdenticalCount && FirstLine) 
				{
					Tempstr=FormatStr(Tempstr,"% 7d ",IdenticalCount);
					Tempstr=HighlightKey(Tempstr, FirstLine, MinField,  MaxField);
				}
			break;

			case FLAG_COUNTS | FLAG_FIRST | FLAG_LAST:
				Tempstr=FormatStr(Tempstr,"% 7d %s",IdenticalCount,PrevLine);
			break;

			case FLAG_DUPLICATES | FLAG_COUNTS:
				if (IdenticalCount > 1) Tempstr=FormatStr(Tempstr,"% 7d %s",IdenticalCount,PrevLine);
			break;


			case FLAG_COUNTS_NOSPACE | FLAG_COUNTS:
			case FLAG_COUNTS_NOSPACE | FLAG_LAST | FLAG_COUNTS:
				if (IdenticalCount && PrevLine) Tempstr=FormatStr(Tempstr,"%d %s",IdenticalCount,PrevLine);
			break;

			case FLAG_COUNTS_NOSPACE | FLAG_HIGHLIGHT_KEY:
			case FLAG_COUNTS_NOSPACE | FLAG_LAST | FLAG_HIGHLIGHT_KEY:
				if (IdenticalCount && PrevLine) 
				{
					Tempstr=FormatStr(Tempstr,"%d ",IdenticalCount);
					Tempstr=HighlightKey(Tempstr, PrevLine, MinField,  MaxField);
				}
			break;

			case FLAG_COUNTS_NOSPACE | FLAG_FIRST:
				if (IdenticalCount) Tempstr=FormatStr(Tempstr,"%d %s",IdenticalCount,FirstLine);
			break;

			case FLAG_COUNTS_NOSPACE | FLAG_FIRST | FLAG_HIGHLIGHT_KEY:
				if (IdenticalCount && FirstLine) 
				{
					Tempstr=FormatStr(Tempstr,"%d ",IdenticalCount);
					Tempstr=HighlightKey(Tempstr, FirstLine, MinField,  MaxField);
				}
			break;

			case FLAG_COUNTS_NOSPACE | FLAG_FIRST | FLAG_LAST:
				Tempstr=FormatStr(Tempstr,"% 7d %s",IdenticalCount,PrevLine);
			break;

			case FLAG_DUPLICATES | FLAG_COUNTS_NOSPACE:
				if (IdenticalCount > 1) Tempstr=FormatStr(Tempstr,"%d %s",IdenticalCount,PrevLine);
			break;

			case FLAG_FIRST | FLAG_LAST:
				if (FirstLine) Tempstr=CopyStr(Tempstr,FirstLine);
			break;

			//FLAG_UNIQ overrides all other flags, so all combinations go to this
			case FLAG_FIRST | FLAG_LAST | FLAG_COUNTS | FLAG_UNIQ:
			case FLAG_LAST | FLAG_COUNTS | FLAG_UNIQ:
			case FLAG_LAST | FLAG_COUNTS_NOSPACE | FLAG_UNIQ:
			case FLAG_UNIQ:
				if (IdenticalCount == 1) Tempstr=CopyStr(Tempstr, PrevLine);
			break;

			case FLAG_DUPLICATES:
				if (IdenticalCount > 1) Tempstr=CopyStr(Tempstr, PrevLine);
			break;

			case FLAG_DUPLICATES | FLAG_HIGHLIGHT_KEY:
				if (IdenticalCount > 1) Tempstr=HighlightKey(Tempstr, PrevLine, MinField,  MaxField);
			break;
		}

		//if (Flags & FLAG_SPACED) write(1,"\n",1);
		if (StrLen(Tempstr)) OutputLine(Tempstr);


Destroy(Tempstr);
}


//If two lines match, this will return a 'line count', which will normally be '1', unless we are
//in 'FLAG_COUNTS_UPDATE' mode, in which case this function will check if the line starts with a 
//number (so may be previous output of uniq -c) and adds to that. If the lines then match, this
//number is returned. If lines do not match '0' is returned
int CompareLines(char *CurrLine, char *PrevLine, int MinField, int MaxField)
{
int result=-1, len1, len2;
char *curr_start, *prev_start;

if (! CurrLine) return(FALSE);
curr_start=CurrLine;
prev_start=PrevLine;

if (! PrevLine) return(FALSE);

len1=FieldStartEnd(&curr_start, MinField, MaxField);
len2=FieldStartEnd(&prev_start, MinField, MaxField);

if (len1 != len2) return(FALSE);
if (len1 < CompareLength) CompareLength=len1;
if (len2 < CompareLength) CompareLength=len2;


switch (Flags & COMPARE_FLAGS)
{
		case 0:
		case FLAG_LENGTH:
			 result=strncmp(curr_start,prev_start,CompareLength); 
		break;

		case FLAG_NOCASE: 
		case FLAG_NOCASE | FLAG_LENGTH: 
			result=strncasecmp(curr_start, prev_start,CompareLength); 
		break;
}

if (result == 0) return(TRUE);
return(FALSE);
}



int ProcessLines(char *CurrLine, char *FirstLine, int MinField, int MaxField, int IdenticalCount)
{
int result;
static char *PrevLine=NULL;

	result=CompareLines(CurrLine,PrevLine,MinField,MaxField);

	//lines do not match, so output
	if (PrevLine && (! result)) OutputLines(CurrLine, PrevLine, FirstLine, MinField, MaxField, IdenticalCount);
	PrevLine=CopyStr(PrevLine, CurrLine);

//Don't do this. Is static!
//Destroy(PrevLine);

return(result);
}


//If in 'update counts' mode, check if the current line starts with a number, then whitespace
//or one of our delimiters
int ExtractLineCount(char **Line)
{
char *ptr;
int LineCount=1, val;

	//numbers can be lead padded with spaces	
	for (ptr=*Line; *ptr==' '; ptr++) ;

	if (isdigit(*ptr))
	{
		val=strtol(ptr,&ptr,10);
		if ((*ptr != '\0') && strchr(Delim, *ptr))
		{
			LineCount=val;
			*Line=ptr+1;
		}
	}

return(LineCount);
}






int main(int argc, char *argv[])
{
char *CurrLine=NULL, *FirstLine=NULL, *start;
int IdenticalCount=0, LineCount=0;
int MinField=1, MaxField=FULL_LINE;

CompareLength=FULL_LINE;
Delim=CopyStr(Delim," 	");
Highlight=CopyStr(Highlight,HL_BOLD);

ParseCommandLine(argc, argv);
GetMinMaxFields(FieldSpecifier, &MinField, &MaxField);

if (StrLen(InPath)) In=fopen(InPath,"r");
else In=stdin;
if (StrLen(OutPath)) Out=fopen(OutPath,"w");
else Out=stdout;


if (! In)
{
	fprintf(stderr,"%s\n","ERROR: Failed to open input");
	exit(1);
}

if (Flags & FLAG_ZERO_TERM) CurrLine=FILEReadLine(CurrLine,In,'\0');
else CurrLine=FILEReadLine(CurrLine,In,'\n');

FirstLine=CopyStr(FirstLine,CurrLine);
while (CurrLine)
{
	StripCRLF(CurrLine);
	start=CurrLine;
	if (Flags & FLAG_COUNTS_UPDATE) LineCount=ExtractLineCount(&start);
	else LineCount=1;
	
	if (! ProcessLines(start, FirstLine, MinField, MaxField, IdenticalCount))
	{
		FirstLine=CopyStr(FirstLine,start);
		IdenticalCount=0;
	}
	IdenticalCount+=LineCount;

if (Flags & FLAG_ZERO_TERM) CurrLine=FILEReadLine(CurrLine,In,'\0');
else CurrLine=FILEReadLine(CurrLine,In,'\n');
}

ProcessLines(CurrLine, FirstLine, MinField, MaxField, IdenticalCount);

Destroy(CurrLine);
Destroy(LineTerminator);

return(0);
}
