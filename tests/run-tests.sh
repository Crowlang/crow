if [ -z "$1" ]
then
	CROW="crow"
else
	CROW="$1"
fi

TESTS=0
PASSED=0

printout() {
	echo '```'
	while read -r line
	do
		echo -e "$line"
	done <<< "$1"
	echo '```'
}

CROW_HASH=`git log -n 1 --pretty=format:"%H" | head -c 7`
echo "Testing Crow version $CROW_HASH"

for file in *.cro
do

	# Get our expected output file name
	expect=`basename -s .cro $file`
	expect="$expect.expect"
	
	# Load our expected output
	expected=`cat $expect`

	# Run our test
	output=`$CROW $file`

	TESTS=`expr $TESTS + 1`

	if [ "$expected" = "$output" ]
	then
		PASSED=`expr $PASSED + 1`
		echo "Test $file passed"	
	else
		echo "Test $file failed"
	fi
done

echo "$PASSED of $TESTS passed"

if [ $PASSED -ne $TESTS ]
then
	exit 1
fi
