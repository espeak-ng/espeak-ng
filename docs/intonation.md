INTONATION {.western}
----------

In eSpeak's standard intonation model, a "tune" is applied to each
clause depending on its punctuation. Other intonation models may be used
for some languages, such as tone languages.

Named tunes are defined in the text file:
`phsource/intonation`{.western}. This file must be compiled for use by
eSpeak by using the espeakedit program, using the menu option:
`Compile -> Compile intonation data`{.western}.

### Clauses {.western}

The tunes which are used for a language can be specified by using a
`tunes`{.western} statement in a voice file in
`espeak-data/voices`{.western}. eg:

`tunes   s1  c1  q1  e1`{.western}

It's parameters are four tune names which are used for clauses which end
in:

1.  2.  3.  4.  

A clause consists of the following parts:

-   -   -   -   

### Tune definitions {.western}

Here is an example tune definition from the file
`phsource/intonation`{.western}.

~~~~ {.western}
tune s1
prehead   46 57
headenv   fall 16
head       4 80 55 -8 -5
headextend 0 63 38 13 0
nucleus  fall 70 18 24 12
nucleus0 fall 64 8
endtune
~~~~

It contains:

**tune** \<tune name\> 
:   Starts the definition of a tune. The `tune     name`{.western} can
    be used in a `tunes`{.western} statements in voice files.
 **endtune** \<tune name\> 
:   Ends the definition of a tune.
 **prehead** \<start pitch\> \<end pitch\> 
:   Gives the pitch path for any series of unstressed syllables before
    the first stressed syllable.
 **headenv** \<envelope\> \<height\> 
:   Gives the pitch envelope which is used for stressed syllables in the
    head (before the nucleus), including `onset`{.western} and
    `headlast`{.western} syllables if these are specified.
    `height`{.western} gives a pitch range for the envelope.
 **head** \<steps\> \<start pitch\> \<end pitch\> \<unstressed start\> \<unstressed end\> 
:   `start pitch`{.western} and `end     pitch`{.western} give a pitch
    path for the stressed syllables of the head. `steps`{.western} is
    the maximum number of stressed syllables for which this applies. If
    there are additional stressed syllables, then the
    `headextend`{.western} statement is used for them.
:   `unstressed start`{.western} and `unstressed     end`{.western} give
    a pitch path for unstressed syllables between two stressed
    syllables. Their values are relative to the pitch of the previous
    stressed syllable. Values are usually negative, meaning that the
    unstressed syllables have lower pitch than the previous stressed
    syllable.
 **headextend** \<percentage list\> 
:   If the head contains more stressed syllables than is specified by
    `steps`{.western}, then `percentage     list`{.western} is used. It
    contains up to 8 numbers which are used repeatedly for the
    additional stressed syllables. A value of 0 corresponds to the lower
    the `start pitch`{.western} and `end pitch`{.western} values of the
    `head`{.western} statement. 100 corresponds to the higher value.
    Negative values and values greater than 100 are allowed.
 **nucleus** \<envelope\> \<top pitch\> \<bottom pitch\> \<tail start\> \<tail end\> 
:   This gives the pitch envelope and pitch range of the last stressed
    syllable of the clause. `tail start`{.western} and
    `tail end`{.western} give a pitch path for the unstressed syllables
    which are after the last stressed syllable.
 **nucleus0** \<envelope\> \<top pitch\> \<bottom pitch\> 
:   This is used instead of `nucleus`{.western} if there are no
    unstressed syllables after the last stressed syllable. In this case,
    the pitch changes of the nucleus and the tail and both included in
    the nucleus.

The following attributes may also be included:

**onset** \<pitch\> \<unstressed start\> \<unstressed end\> 
:   This specifies the pitch for the first stressed syllable of the
    head. If the `onset`{.western} statement is present, then the
    `head`{.western} statement used for the stressed syllables after the
    first.
 **headlast** \<pitch\> \<unstressed start\> \<unstressed end\> 
:   This specifies the pitch for the last stressed syllable of the head
    (i.e. the stressed syllable before the nucleus).

