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

def fold_lines(path):
	next_line = None
	with open(path) as f:
		for line in f:
			line = line.replace('\n', '')
			if line.startswith(' '):
				next_line = '%s%s' % (next_line, line[1:])
				continue
			if next_line:
				yield next_line
			next_line = line

def iana_subtag_entries(path):
	tag = {}
	for line in fold_lines(path):
		if line == '%%':
			if 'Type' in tag:
				yield tag
			tag = {}
			continue

		packed = line.split(': ')
		key    = packed[0]
		value  = ': '.join(packed[1:])

		if key == 'Description':
			# Only select the first Description. This handles subtag codes
			# that have multiple descriptions (e.g. 'es' maps to "Spanish"
			# and "Castilian").
			if not key in tag.keys():
				tag[key] = value
		else:
			tag[key] = value
	yield tag

typemap = {
	'extlang':       'ExtLang',
	'grandfathered': 'Grandfathered',
	'language':      'Language',
	'redundant':     'Redundant',
	'region':        'Region',
	'script':        'Script',
	'variant':       'Variant',
}

scopemap = {
	'collection':    'Collection',
	'macrolanguage': 'MacroLanguage',
	'special':       'Special',
	'private-use':   'PrivateUse',
}

def read_iana_subtags(path):
	tags = {}
	for tag in iana_subtag_entries(path):
		if 'Subtag' in tag.keys():
			ref = tag['Subtag']
			del tag['Subtag']
		else:
			ref = tag['Tag']
			del tag['Tag']

		if 'Scope' in tag.keys():
			if tag['Type'] != 'language':
				raise Exception('"Scope" property unexpected for Type="%s"' % tag['Type'])

			tag['Type'] = scopemap[ tag['Scope'] ]
			del tag['Scope']
		else:
			tag['Type'] = typemap[ tag['Type'] ]

		if '..' not in ref: # exclude private use definitions
			tags[ref] = tag
	return tags
