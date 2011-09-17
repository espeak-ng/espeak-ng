eSpeak is a compact, multi-language, open source
text-to-speech synthesizer.

This version is a SAPI5 compatible Windows speech engine
which should work with screen readers such as Jaws,
NVDA, and Window-Eyes.

There is also a version of eSpeak which can be run as a
command-line program.  This is in eSpeak\command-line.
Read docs\commands.html for details.



Voices and Languages
====================

The available Voices can be seen as files in the directory
  espeak-edit/voices.

To change which eSpeak Voices are available through
Windows, re-run the installer and specify up to six
Voice files which you want to use.

The tone of a Voice can be modified by adding a variant
name after the Voice name, eg:
  pt+f3

The available variants are:
male:    +m1  +m2  +m3  +m4  +m5
female:  +f1  +f2  +f3  +f4
other effects:  +croak  +wisper

These variants are defined by text files in
  espeak-edit/voices/!v


Updates
=======

The eSpeak project homepage is at:
  http://espeak.sourceforge.net/

Comments, corrections, and other feedback and assistance
is sought from native speakers of the various languages
because I've no idea how they are supposed to sound :-)

To make changes to pronunciation rules and exceptions,
or to change the sounds of phonemes, or just to experiment
with speech synthesis, download the "espeakedit" program.


License
=======

This software is licencsed under the GNU General Public License
version3.  See file:  License.txt.
