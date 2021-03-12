# Numbers

- [Language Options](#language-options)

## Language Options

The following properties in `tr_languages.c` control how numbers behave for a
language:

    tr->langopts.numbers
    tr->langopts.numbers2

These controls how numbers are pronounced.

If `numbers` is set to `0`, numbers will not be pronounced.
Setting it to `1` (the default value) will enable number pronunciation using the dictionary rules.
For more control over number pronunciation, see the flags in `translate.h`.


    tr->langopts.max_digits

This is the maximum number of digits that can be spoken by the language. If the
number of digits is larger than this, the number is spoken digit by digit.

    tr->langopts.break_numbers

This controls how digits are grouped for speaking large numbers. The default is
to group into 3 digit groups (thousand, million, billion, trillion, etc.).

    tr->langopts.thousands_sep

The character used to separate digit groups. The default is `,`.

    tr->langopts.decimal_sep

The character used to separate the integer and decimal parts of a real number.
The default is `.`.

