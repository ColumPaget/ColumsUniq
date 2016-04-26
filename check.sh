#! /bin/sh

EXIT=0

Title()
{
echo -e "\n\033[01m ############### $* \033[00;39m"
}


OkayMessage()
{
echo -e "  \033[01;32mOKAY\033[00;39m    $1"
}

FailMessage()
{
echo -e "  \033[01;31mFAIL\033[00;39m    $1"
EXIT=3    # Bash says 1 and 2 are reserved for specific errors
}

CompareAgainstStandardUniq()
{
OLD=`cat $2 | uniq $1`
NEW=`cat $2 | ./cuniq $1`

if [ "$OLD" = "$NEW" ]
then
	OkayMessage "$3 works"
else
	FailMessage "$3 FAILED"
fi

}




Title "Testing 'cuniq'"
CompareAgainstStandardUniq "" tests/uniq.1 "Standard use (cat file | uniq)"
CompareAgainstStandardUniq -c tests/uniq.1 "Counts (uniq -c)"
CompareAgainstStandardUniq -i tests/uniq.2 "Ignore case (uniq -i)"
CompareAgainstStandardUniq -u tests/uniq.2 "Unique Lines only (uniq -u)"
CompareAgainstStandardUniq -d tests/uniq.2 "Duplicate Lines only (uniq -d)"
CompareAgainstStandardUniq "-s 2" tests/uniq.2 "Skip initial n chars (uniq -s <x>)"
CompareAgainstStandardUniq "-w 5" tests/uniq.2 "Match only n chars (uniq -w <x>)"
CompareAgainstStandardUniq "-s 1 -w 5" tests/uniq.2 "Skip x, Match y chars (uniq -s <x> -w <y>)"
CompareAgainstStandardUniq "-z" tests/uniq.5 "Zero Terminated lines"


UPDATE_COUNTS_EXPECTED=`echo -e "7 apples\n2 oranges\n1 Oranges\n2 oranges\n1 OrangeS\n2 oranges\n1 orangeS\n1 oranges\n1 ORANGES\n1 pear\n6 grape\n1 banana\n"`
UPDATE_COUNTS_OUTPUT=`cat tests/uniq.3 | ./cuniq -C -n`
if [ "$UPDATE_COUNTS_OUTPUT" = "$UPDATE_COUNTS_EXPECTED" ]
then
	OkayMessage "Updating counts from previous uniq works"
else
	FailMessage "Updating counts from previous uniq FAILED"
fi


ZT_EXPECTED=af76c4637ec57f9ff3abffcbbaddc4f1
ZT_OUTPUT=`cat tests/uniq.5 | ./cuniq -z | md5sum | cut -d ' ' -f 1`
if [ "$ZT_OUTPUT" = "$ZT_EXPECTED" ]
then
	OkayMessage "Zero termination (uniq -z) works as expected, (may be different from your installed uniq)"
else
	FailMessage "Zero termination (uniq -z) FAILED"
fi


ZT_EXPECTED="apples; oranges; pear; grape; banana; "
ZT_OUTPUT=`cat tests/uniq.1 | ./cuniq -L "; "`
if [ "$ZT_OUTPUT" = "$ZT_EXPECTED" ]
then
	OkayMessage "Specifying line terminators works"
else
	FailMessage "Specifying line terminators FAILED"
fi

ZT_EXPECTED=9d115631a3b01782bb33fb0727bcd3b0
ZT_OUTPUT=`cat tests/uniq.6 | ./cuniq -i -t ",;" -f 2 | md5sum | cut -d ' ' -f 1`
if [ "$ZT_OUTPUT" = "$ZT_EXPECTED" ]
then
	OkayMessage "Field matching with multiple delimiters works"
else
	FailMessage "Field matching with multiple delimiters FAILED"
fi

ZT_EXPECTED=c74e2e66b92510a32fdc0298a13ac17b
ZT_OUTPUT=`cat tests/uniq.6 | ./cuniq -i -t ",;" -f 2 | md5sum | cut -d ' ' -f 1`
if [ "$ZT_OUTPUT" = "$ZT_EXPECTED" ]
then
	OkayMessage "Output LAST line of a matching group works"
else
	OkayMessage "Output LAST line of a matching group FAILED"
fi



ZT_EXPECTED=`echo -e "apples\r\noranges\r\npear\r\ngrape\r\nbanana\r\n"`
ZT_OUTPUT=`cat tests/uniq.1 | ./cuniq -L "\r\n"`
if [ "$ZT_OUTPUT" = "$ZT_EXPECTED" ]
then
	OkayMessage "Specifying line terminators with quoted characters (\\\r \\\n etc) works"
else
	FailMessage "Specifying line terminators with quoted characters (\\\r \\\n etc) FAILED"
fi


ZT_EXPECTED=`echo -e "apples\r\noranges\r\npear\r\ngrape\r\nbanana\r\n"`
ZT_OUTPUT=`./cuniq -L "\r\n" tests/uniq.1`
if [ "$ZT_OUTPUT" = "$ZT_EXPECTED" ]
then
	OkayMessage "Input file specified on command-line works"
else
	FailMessage "Input file specified on command-line FAILED"
fi


ZT_EXPECTED=`echo -e "apples\r\noranges\r\npear\r\ngrape\r\nbanana\r\n"`
ZT_OUTPUT=`./cuniq -L "\r\n" tests/uniq.1 tests/test.out; cat tests/test.out`
if [ "$ZT_OUTPUT" = "$ZT_EXPECTED" ]
then
	OkayMessage "Input file and output file specified on command-line works"
else
	FailMessage "Input file and output file specified on command-line FAILED"
fi

exit $EXIT
