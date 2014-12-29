#! /bin/bash

make clean
make
echo

./aric -c "$1" compressed
./aric -d compressed decompressed
echo

# compare original and decompressed
if diff "$1" decompressed >/dev/null
then
	echo "HUBICKU!"
else
	echo "NASRAT!"
fi

rm compressed decompressed
