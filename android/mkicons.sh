#!/bin/bash

rsvg -w 24 -h 24 icons/launcher.svg res/drawable-ldpi/icon.png
rsvg -w 32 -h 32 icons/launcher.svg res/drawable-mdpi/icon.png
rsvg -w 48 -h 48 icons/launcher.svg res/drawable-hdpi/icon.png
rsvg -w 64 -h 64 icons/launcher.svg res/drawable-xhdpi/icon.png

ls icons/Flag_*.svg | while read flag ; do
	rsvg -w 75 -h 50 $flag `echo $flag | sed -e 's,.svg,.png,g'`
done

rsvg -w 1024 -h 500 icons/feature-graphic.svg icons/feature-graphic.png
rsvg -w 180 -h 120 icons/promo-graphic.svg icons/promo-graphic.png
