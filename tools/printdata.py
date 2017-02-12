#!/usr/bin/python

# Copyright (C) 2012-2017 Reece H. Dunn
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

null = ucd.CodePoint('0000')

unicode_chars = {}
for data in ucd.parse_ucd_data(ucd_rootdir, 'UnicodeData'):
	for codepoint in data['CodePoint']:
		unicode_chars[codepoint] = data
for propfile in ['PropList', 'DerivedCoreProperties']:
	for data in ucd.parse_ucd_data(ucd_rootdir, propfile):
		for codepoint in data['Range']:
			try:
				unicode_chars[codepoint][data['Property']] = 1
			except KeyError:
				unicode_chars[codepoint] = {'CodePoint': codepoint}
				unicode_chars[codepoint][data['Property']] = 1
for data in ucd.parse_ucd_data(ucd_rootdir, 'Scripts'):
	for codepoint in data['Range']:
		unicode_chars[codepoint]['Script'] = data['Script']
if '--with-csur' in sys.argv:
	for csur in ['Klingon']:
		for data in ucd.parse_ucd_data('data/csur', csur):
			for codepoint in data['CodePoint']:
				unicode_chars[codepoint] = data

def iscntrl(data):
	return 1 if data.get('Name', '') == '<control>' else 0

def isdigit(data):
	return 1 if data['CodePoint'].char() in '0123456789' else 0

def isxdigit(data):
	return 1 if data['CodePoint'].char() in '0123456789ABCDEFabcdef' else 0

def isspace(data):
	if data.get('White_Space', 0):
		dt = data.get('DecompositionType', '')
		return 1 if dt == None or not dt.startswith('<noBreak>') else 0
	else:
		return 0

def isblank(data): # word separator
	if data.get('GeneralCategory', 'Cn') == 'Zs' or data['CodePoint'].char() == '\t':
		dt = data.get('DecompositionType', '')
		return 1 if dt == None or not dt.startswith('<noBreak>') else 0
	else:
		return 0

def ispunct(data):
	if data.get('GeneralCategory', 'Cn')[0] in 'P':
		return 1
	else:
		return 0

def isprint(data):
	if data.get('GeneralCategory', 'Cn')[0] in 'LMNPSZ': # not in 'CI'
		return 1
	else:
		return 0

def isgraph(data):
	if data.get('GeneralCategory', 'Cn')[0] in 'LMNPS': # not in 'CZI'
		return 1
	else:
		return 0

def isalnum(data):
	if data.get('GeneralCategory', 'Cn')[0] in 'N':
		return 1
	else:
		return data.get('Alphabetic', 0)

def isalpha(data):
	return data.get('Alphabetic', 0)

def isupper(data):
	if data.get('Uppercase', 0):
		return 1
	elif data.get('LowerCase', null) != null: # Some Lt characters have lowercase forms.
		return 1
	else:
		return 0

def islower(data):
	if data.get('Lowercase', 0):
		return 1
	elif data.get('UpperCase', null) != null:
		return 1
	else:
		return 0

if __name__ == '__main__':
	for codepoint in ucd.CodeRange('000000..10FFFF'):
		try:
			data = unicode_chars[codepoint]
		except KeyError:
			data = {'CodePoint': codepoint}
		script = data.get('Script', 'Zzzz')
		title = data.get('TitleCase', codepoint)
		upper = data.get('UpperCase', codepoint)
		lower = data.get('LowerCase', codepoint)
		if title == null: title = codepoint
		if upper == null: upper = codepoint
		if lower == null: lower = codepoint
		print('%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s' % (
		      codepoint, script,
		      data.get('GeneralCategory', 'Cn')[0], data.get('GeneralCategory', 'Cn'),
		      upper, lower, title,
		      isdigit(data), isxdigit(data),
		      iscntrl(data), isspace(data), isblank(data), ispunct(data),
		      isprint(data), isgraph(data), isalnum(data), isalpha(data), isupper(data), islower(data)))
