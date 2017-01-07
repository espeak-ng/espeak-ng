#!/usr/bin/python

# Copyright (C) 2012 Reece H. Dunn
#
# This file is part of ucd-tools.
#
# ucd-tools is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# ucd-tools is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with ucd-tools.  If not, see <http://www.gnu.org/licenses/>.

import os
import sys
import ucd

ucd_rootdir = sys.argv[1]
csur_rootdir = 'data/csur'

unicode_chars = {}
for data in ucd.parse_ucd_data(ucd_rootdir, 'UnicodeData'):
	for codepoint in data['CodePoint']:
		unicode_chars[codepoint] = data
		unicode_chars[codepoint]['Properties'] = []
for data in ucd.parse_ucd_data(ucd_rootdir, 'PropList'):
	if data['Property'] in ['White_Space']:
		for codepoint in data['Range']:
			unicode_chars[codepoint]['Properties'].append(data['Property'])
for data in ucd.parse_ucd_data(ucd_rootdir, 'Scripts'):
	for codepoint in data['Range']:
		unicode_chars[codepoint]['Script'] = data['Script']
if '--with-csur' in sys.argv:
	for csur in ['Klingon']:
		for data in ucd.parse_ucd_data('data/csur', csur):
			for codepoint in data['CodePoint']:
				if not 'TitleCase'  in data: data['TitleCase']  = codepoint
				if not 'UpperCase'  in data: data['UpperCase']  = codepoint
				if not 'LowerCase'  in data: data['LowerCase']  = codepoint
				if not 'Properties' in data: data['Properties'] = []
				unicode_chars[codepoint] = data

null = ucd.CodePoint('0000')
if __name__ == '__main__':
	for codepoint in ucd.CodeRange('000000..10FFFF'):
		try:
			data = unicode_chars[codepoint]
		except KeyError:
			data = {'GeneralCategory': 'Cn', 'TitleCase': codepoint, 'UpperCase': codepoint, 'LowerCase': codepoint, 'Properties': []}
		try:
			script = data['Script']
		except KeyError:
			script = 'Zzzz'
		title = data['TitleCase']
		upper = data['UpperCase']
		lower = data['LowerCase']
		if title == null: title = codepoint
		if upper == null: upper = codepoint
		if lower == null: lower = codepoint
		print('%s %s %s %s %s %s %s %s' % (
		      codepoint, script,
		      data['GeneralCategory'][0], data['GeneralCategory'],
		      upper, lower, title,
		      ' '.join(data['Properties'])))
