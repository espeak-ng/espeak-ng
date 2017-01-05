#!/bin/bash

RSVG=${RSVG:=rsvg-convert}

${RSVG} -w 24 -h 24 icons/launcher.svg > res/drawable-ldpi/icon.png
${RSVG} -w 32 -h 32 icons/launcher.svg > res/drawable-mdpi/icon.png
${RSVG} -w 48 -h 48 icons/launcher.svg > res/drawable-hdpi/icon.png
${RSVG} -w 64 -h 64 icons/launcher.svg > res/drawable-xhdpi/icon.png

${RSVG} -w 180 -h 120 icons/promo-graphic.svg > icons/promo-graphic.png
