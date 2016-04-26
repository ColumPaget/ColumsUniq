#cuniq - COLUM'S UNIQ

cuniq is a unix 'uniq' command with a couple of extra features (and a couple of features missing). cuniq supports multiple delimiters, matching any field within the input, updating the counts of a previous uniq, selectinmg the last or first of a block of machines lines, skipping both fields and characters.


#EXAMPLES

use [ and ] as delimiters to break input into fields, match on field 2

`cat input | cuniq -t "[]" -F 2`

break input up into fields using ', ' as a delimiter. Perform uniq on the third field of the line 
(for lines with matching 3rd fields only the first line is displayed)

`cat input | cuniq -t , -F 3`

as above, but only display last line of a matching group

`cat input | cuniq -t , -f 3 -Z`

skip 15 characters, and only display unique lines (lines that occur once)

`cat input | cuniq -s 15 -u`

only display lines that have a unique third field

`cat input | cuniq -F 3 -u`

match on the the third field, with ',' and ';' acting as field delimiters

`cat input | cuniq -t ",;" -F 3`

match starting at 6 bytes in (skip six bytes) and 10 bytes length

`cat input | cuniq -s 6 -w 10`

ignore case, output last line of a collated block

`cat input | cuinq -i -Z`


#SYNTAX
Usage: cuniq [OPTION]... [INPUT [OUTPUT]]
Filter adjacent matching lines from INPUT (or standard input),
writing to OUTPUT (or standard output).

With no options, matching lines are merged to the first occurrence.

Mandatory arguments to long options are mandatory for short options too.
  -c, --count           prefix lines by the number of occurrences
  -C, --increment       prefix lines by the number of occurrences, handle input lines that already contain counts
  -d, --repeated        only print duplicate lines
  -D, --all-repeated[=delimit-method]  print all duplicate lines
                        delimit-method={none(default),prepend,separate}
                        Delimiting is done with blank lines
  -f, --skip-fields=N   avoid comparing the first N fields
  -i, --ignore-case     ignore differences in case when comparing
  -n,                   suppress leading spaces of counts
  -s, --skip-chars=N    avoid comparing the first N characters
  -t, --delims=[delimters]  list of characters treated as field delimiters
  -F, --field=N         compare only this field on each line
  -u, --unique          only print unique lines
  -z, --zero-terminated  end lines with 0 byte, not newline. This applies to both input and output. Output termination can be overridden with --endline
  -L, --endline=[string] Terminate lines of output with the given string
  -w, --check-chars=N   compare no more than N characters in lines
  -1, --first           When collating lines, display the FIRST line
  -Z, --last            When collating lines, display the LAST line
  -?, --help     display this help and exit
      --version  output version information and exit

Fields are broken up by delimiter characters specified by the -t or --delims options. The default delimters are tab and space.
Some options cause uniq to consider strings with different case as identical, or to consider substrings, rather than the entire string, so that two strings can be considered identical because their selected characters match. In these cases the -1 and -Z flags can be used to select the first or last occurance of a matching series.

Note: 'uniq' does not detect repeated lines unless they are adjacent.
You may want to sort the input first, or use 'sort -u' without 'uniq'.
(-i, -s, -w, -f and -F can cause non identical lines to be collated together) 

Report bugs to colums.projects@gmail.com
