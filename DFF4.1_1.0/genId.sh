#!/bin/sh

i=0

# increment
#i=$((i+1))

echo "i=$i"
cat $1 > $2

# Param
while grep -q "\{Px\}" $2
do
   echo "replace P $i"
   awk -vi=$i 'NR==1,/\{Px\}/{sub(/\{Px\}/, i)} 1' $2 > tmp
   mv tmp $2
   # increment
   i=$((i+1))
done   

# Comobj
i=0
echo "i=$i"
while grep -q "\{Cx\}" $2
do
   echo "replace C $i"
   awk -vi=$i 'NR==1,/\{Cx\}/{sub(/\{Cx\}/, i)} 1' $2 > tmp
   mv tmp $2
   # increment
   i=$((i+1))
done

# G????
i=0
echo "i=$i"
while grep -q "\{Gx\}" $2
do
   echo "replace G $i"
   awk -vi=$i 'NR==1,/\{Gx\}/{sub(/\{Gx\}/, i)} 1' $2 > tmp
   mv tmp $2
   # increment
   i=$((i+1))
done
