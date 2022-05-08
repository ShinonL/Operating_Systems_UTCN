# functions
list_files() {
    ls $1
    ALLFILES=$( ls $1 | wc -l)
}

list_pattern_files() {
    ls $1 | grep -E ^'f' | grep -E '.c'$
    FFILES=$(ls $1 | grep -E ^'f' | grep -E '.c'$ | wc -l)
}

list_subdirs() {
    ls -d
    DIR=$(ls -d | wc -l)
}


# sequential execution
if test $# -ne 1
then
	echo USAGE: $0 not enough parameters
	exit
fi

echo $1

if test ! -d $1
then
	echo The parameter must be a directory
	exit
fi

cd "$1"
list_files
list_pattern_files
list_subdirs

mkdir "DIR"
cd "./DIR"
touch "ALLFILES"
echo $FFILES >> "ALLFILES"
