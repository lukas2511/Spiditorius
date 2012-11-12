#!/bin/bash

echo '#include <RLEBitmap.h>' > src/Sprites.c
echo >> src/Sprites.c

echo '#ifndef __SPRITES_H__' > inc/Sprites.h
echo '#define __SPRITES_H__' >> inc/Sprites.h
echo '#include <RLEBitmap.h>' >> inc/Sprites.h

for asset in assets/*.png
do
	name="$(basename $asset | cut -d. -f1)"

	PNGConverter $asset RLEBitmap $name >> src/Sprites.c
	echo >> src/Sprites.c

	echo 'extern const RLEBitmap *const '$name >> inc/Sprites.h
done

echo '#endif' >> inc/Sprites.h

