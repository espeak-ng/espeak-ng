# Text Markup

- [SSML: Speech Synthesis Markup Language](#ssml-speech-synthesis-markup-language)
- [HTML](#html)

-----

## SSML: Speech Synthesis Markup Language

The following markup tags and attributes are recognised:

**\<speak\>**

*   xml:base   (the value is just passed back as a parameter with the UriCallback() function)
*   xml:lang

**\<voice\>**

*   xml:lang
*   name
*   age
*   variant
*   gender

**\<prosody\>**

*   rate
*   volume
*   pitch
*   range

**\<say-as\>**

*   interpret-as="characters"
*   interpret-as="characters"  format="glyphs"
*   interpret-as="tts:key"
*   interpret-as="tts:char"
*   interpret-as="tts:digits"

**\<mark\>** name

**\<s\>**

*   xml:lang

**\<p\>**

*   xml:lang

**\<sub\>** alias

**\<tts:style\>**

*   field="punctuation"   mode=none,all,some
*   field="capital\_letters"   mode=no,spelling,icon,pitch

**\<audio\>** src

**\<emphasis\>**

*   level

**\<break\>**

*   strength
*   time

## HTML

eSpeak can speak HTML text directly, or text containing both SSML and HTML markup.  
Any unrecognised tags are ignored.

The following tags case a sentence break.  
**\<br\>  \<dd\>   \<li\>   \<img\>   \<td\>**

The following tags case a paragraph break.  
**\<h1\>   \<h2\>   \<h3\>   \<h4\>   \<hr\>**

Text between the following tags is ignored.  
**\<script\>   ...   \</script\>**  
**\<style\>   ...   \</style\>**
