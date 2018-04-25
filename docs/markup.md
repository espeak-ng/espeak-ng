# SSML and HTML Support

- [SSML (Speech Synthesis Markup Language)](#ssml-speech-synthesis-markup-language)
  - [speak](#speak)
  - [voice](#voice)
  - [prosody](#prosody)
  - [say-as](#say-as)
  - [mark](#mark)
  - [s](#s)
  - [p](#p)
  - [sub](#sub)
  - [tts:style](#tts-style)
  - [audio](#audio)
  - [emphasis](#emphasis)
  - [break](#break)
- [HTML](#html)
- [References](#references)
  - [SSML](#ssml)
  - [HTML](#html-1)

-----

## SSML (Speech Synthesis Markup Language)

The following markup tags and attributes are recognised:

### speak

*   xml:base   (the value is just passed back as a parameter with the UriCallback() function)
*   xml:lang

### voice

*   xml:lang
*   name
*   age
*   variant
*   gender

### prosody

*   rate
*   volume
*   pitch
*   range

### say-as

*   interpret-as="characters"
*   interpret-as="characters"  format="glyphs"
*   interpret-as="tts:key"
*   interpret-as="tts:char"
*   interpret-as="tts:digits"

### mark

*   name

### s

*   xml:lang

### p

*   xml:lang

### sub

*   alias

### tts:style

*   field="punctuation"   mode=none,all,some
*   field="capital\_letters"   mode=no,spelling,icon,pitch

### audio

*   src

### emphasis

*   level

### break

*   strength
*   time

## HTML

eSpeak can speak HTML text directly, or text containing both SSML and HTML markup.  
Any unrecognised tags are ignored.

The following tags cause a sentence break:

*   `br`
*   `dd`
*   `li`
*   `img`
*   `td`

The following tags cause a paragraph break:

*   `h1`
*   `h2`
*   `h3`
*   `h4`
*   `hr`

Text between the following tags is ignored:

*   `script`
*   `style`

## References

### SSML

1. [Speech Synthesis Markup Language (SSML) Version 1.0](https://www.w3.org/TR/2009/REC-speech-synthesis-20090303/).
   W3C Recommendation, 3 March 2009. W3C.
1. [Speech Synthesis Markup Language (SSML) Version 1.1](http://www.w3.org/TR/2010/REC-speech-synthesis11-20100907/).
   W3C Recommendation, 7 September 2010. W3C.
1. [SSML 1.0 say-as attribute values](http://www.w3.org/TR/2005/NOTE-ssml-sayas-20050526).
   W3C NOTE, 26 May 2005. W3C.

### HTML

1.  [HTML 5.2](https://www.w3.org/TR/2017/REC-html52-20171214/).
    W3C Recommendation, 14 December 2017. W3C.
1.  [HTML Living Standard](https://html.spec.whatwg.org/multipage/).
    Continually updated. WHATWG.
