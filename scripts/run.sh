#!/bin/sh
set -e
./build/apps/bezline/bezline
./build/apps/bmpshow/bmpshow
./build/apps/cardioid/cardioid
./build/apps/circles/circles
./build/apps/fill/fill
./build/apps/fonts/fonts
./build/apps/histgram/histgram
./build/apps/lines/lines
./build/apps/primes/primes
./build/apps/rect/rect
./build/apps/roundrct/roundrct
./build/apps/waggle/waggle

convert bezline.bmp images/bezline.png
convert bmpshow.bmp images/bmpshow.png
convert cardioid.bmp images/cardioid.png
convert circles.bmp images/circles.png
convert fill.bmp images/fill.png
convert fonts.bmp images/fonts.png
convert histgram.bmp images/histgram.png
convert lines.bmp images/lines.png
convert primes.bmp images/primes.png
convert rect.bmp images/rect.png
convert roundrct.bmp images/roundrct.png
convert waggle.bmp images/waggle.png
