#!/bin/bash

echo '#include <RLEBitmap.h>' > src/Sprites.c
echo >> src/Sprites.c
for asset in assets/*.png
do
	name="$(basename $asset | cut -d. -f1)"
	PNGConverter $asset RLEBitmap $name >> src/Sprites.c
	echo >> src/Sprites.c
done
