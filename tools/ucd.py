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
import iana

script_map = {
	# UCD script names not derivable from IANA script tags:
	'Canadian_Aboriginal': 'Cans',
	'Common': 'Zyyy',
	'Egyptian_Hieroglyphs': 'Egyp',
	'Inherited': 'Zyyy',
	'Meetei_Mayek': 'Mtei',
	'Nko': 'Nkoo',
	'Phags_Pa': 'Phag',
	# Codes in http://www.unicode.org/iso15924/iso15924-codes.html not in IANA:
	'Cuneiform': 'Xsux',
}
for ref, tag in iana.read_iana_subtags('data/language-subtag-registry').items():
	if tag['Type'] == 'Script':
		# Convert the IANA scipt tag descriptions to the UCD script names:
		desc = tag['Description']
		if ' (' in desc:
			desc = desc.split(' (')[0]
		desc = desc.replace(' ', '_')
		script_map[desc] = ref
# Fix up incorrectly mapped script names:
script_map['Cyrillic'] = 'Cyrl'

class CodePoint:
	def __init__(self, x):
		if isinstance(x, str):
			self.codepoint = int(x, 16)
		else:
			self.codepoint = x

	def __repr__(self):
		return '%06X' % self.codepoint

	def __str__(self):
		return '%06X' % self.codepoint

	def __iter__(self):
		yield self

	def __hash__(self):
		return self.codepoint

	def __eq__(self, other):
		return self.codepoint == other.codepoint

	def __ne__(self, other):
		return self.codepoint != other.codepoint

	def __lt__(self, other):
		return self.codepoint < other.codepoint

class CodeRange:
	def __init__(self, x):
		f, l = x.split('..')
		self.first = CodePoint(f)
		self.last  = CodePoint(l)

	def __repr__(self):
		return '%s..%s' % (self.first, self.last)

	def __str__(self):
		return '%s..%s' % (self.first, self.last)

	def __iter__(self):
		for c in range(self.first.codepoint, self.last.codepoint + 1):
			yield CodePoint(c)

	def size(self):
		return self.last.codepoint - self.first.codepoint + 1

def codepoint(x):
	if '..' in x:
		return CodeRange(x)
	if ' ' in x:
		return [CodePoint(c) for c in x.split()]
	if x == '':
		return CodePoint('0000')
	return CodePoint(x)

def string(x):
	if x == '':
		return None
	return x

def boolean(x):
	if x == 'Y':
		return True
	return False

def script(x):
	return script_map[x]

data_items = {
	'Blocks': [
		('Range', codepoint),
		('Name', str)
	],
	'DerivedAge': [
		('Range', codepoint),
		('Age', str),
	],
	'PropList': [
		('Range', codepoint),
		('Property', str),
	],
	'Scripts': [
		('Range', codepoint),
		('Script', script),
	],
	'UnicodeData': [
		('CodePoint', codepoint),
		('Name', string),
		('GeneralCategory', string),
		('CanonicalCombiningClass', int),
		('BidiClass', string),
		('DecompositionType', string),
		('DecompositionMapping', string),
		('NumericType', string),
		('NumericValue', string),
		('BidiMirrored', boolean),
		('UnicodeName', string),
		('ISOComment', string),
		('UpperCase', codepoint),
		('LowerCase', codepoint),
		('TitleCase', codepoint),
	],
	# Supplemental Data:
	'Klingon': [
		('CodePoint', codepoint),
		('Script', str),
		('GeneralCategory', string),
		('Name', string),
		('Transliteration', string),
	],
}

def parse_ucd_data(ucd_rootdir, dataset):
	keys  = data_items[dataset]
	first = None
	with open(os.path.join(ucd_rootdir, '%s.txt' % dataset)) as f:
		for line in f:
			line = line.replace('\n', '').split('#')[0]
			linedata = [' '.join(x.split()) for x in line.split(';')]
			if len(linedata) == len(keys):
				if linedata[1].endswith(', First>'):
					first = linedata
					continue

				if linedata[1].endswith(', Last>'):
					linedata[0] = '%s..%s' % (first[0], linedata[0])
					linedata[1] = linedata[1].replace(', Last>', '').replace('<', '')
					first = None

				data = {}
				for keydata, value in zip(keys, linedata):
					key, typemap = keydata
					if key:
						data[key] = typemap(value)
				yield data

if __name__ == '__main__':
	try:
		items = sys.argv[3].split(',')
	except:
		items = None
	for entry in parse_ucd_data(sys.argv[1], sys.argv[2]):
		if items:
			print ','.join([str(entry[item]) for item in items])
		else:
			print entry
