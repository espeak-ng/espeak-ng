#!/bin/bash
#
# Copyright (C) 2013 Reece H. Dunn
# License: GPLv3+
#
# Helper utility for removing a string/string-list item in all string resources.
#
# Usage: ./remove_string.sh <string_id>

ls res/values*/strings.xml | while read STRINGS ; do
	xmlstarlet ed -P -d "/resources/*[@name='${1}']" ${STRINGS} > /tmp/strings.xml
	mv /tmp/strings.xml ${STRINGS}
done
