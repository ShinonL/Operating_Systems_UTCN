# functions

# check correctness of the call
if test $# -ne 2
then
	echo USAGE: $0 needs 2 parameters: first the directory name and then the name of the searched file
	exit
fi

# check if the given directory exists
if test ! -d $1
then
	echo The first parameter must be a directory
	exit
fi

find $1 -type d,f -name "${2}*"

