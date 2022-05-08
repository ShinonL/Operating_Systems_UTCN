# functions
get_stat() {
    echo "Owner: $(stat -c %U $1)"  
    echo "Size: $(stat -c %s $1)"

}

function recursive_path() {

	for elem in $1/*
	do
		if test -d "$elem"
		then
			recursive_path "$elem" $2
        else 
            if test $(stat -c %U $1) = $2
            then echo $elem
            fi
		fi		
	done
	
	for elem in $1/.*
	do
		if test "`basename $elem`" = "." -o "`basename $elem`" = ".."
		then
			continue
		else
			if test -d "$elem"
			then
				recursive_path "$elem" $2
            else 
                if test $(stat -c %U $1) = $2
                then echo $elem
                fi
			fi
		fi
	done
}

# check parameter number
if test $# -ne 3 && test $# -ne 1 
then 
    echo USAGE: For 1. eneter 1 parameter, for 2. enter 3 parameters as specified in the problem
    exit
fi

# subpunctul 1.
if test $# -eq 1
    then get_stat $1
fi

#subpunctul 2 folosind functia data ca exemplu
recursive_path $1 $2

#subpunctul 2 in o linie
echo "A simpler idea:"
find $1 -type f -user $2
