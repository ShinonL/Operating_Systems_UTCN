declare -i index
for((index=2; index<=$#; index+= $1))
do
   echo ${!index}
done
