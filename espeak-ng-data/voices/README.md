# Voices

The voices in espeak-ng are grouped by their
[ISO 639-5](https://en.wikipedia.org/wiki/List_of_ISO_639-5_codes) language
family code. The voices are identified by their
[BCP 47](https://en.wikipedia.org/wiki/BCP47) identifier. For several accents
and dialects,
[private-use extensions](https://raw.githubusercontent.com/espeak-ng/bcp47-data/master/bcp47-extensions)
have been used.

The supported languages are:

| Family Code | Voice Identifier  | Language Family       | Language                    | Accent/Dialect         |
|-------------|-------------------|-----------------------|-----------------------------|------------------------|
| `gmw`       | `af`              | West Germanic         | Afrikaans                   |                        |
| `ine`       | `sq`              | Indo-European         | Albanian                    |                        |
| `sem`       | `am`              | Semitic               | Amharic                     |                        |
| `roa`       | `an`              | Romance               | Aragonese                   |                        |
| `ine`       | `hy`              | Indo-European         | Armenian                    | East Armenian          |
| `ine`       | `hy-arevmda`      | Indo-European         | Armenian                    | West Armenian          |
| `inc`       | `as`              | Indic                 | Assamese                    |                        |
| `trk`       | `az`              | Turkic                | Azerbaijani                 |                        |
|             | `eu`              |                       | Basque                      |                        |
| `inc`       | `bn`              | Indic                 | Bengali                     |                        |
| `zls`       | `bs`              | South Slavic          | Bosnian                     |                        |
| `zls`       | `bg`              | South Slavic          | Bulgarian                   |                        |
| `sit`       | `my`              | Sino-Tibetan          | Burmese                     |                        |
| `roa`       | `ca`              | Romance               | Catalan                     |                        |
| `sit`       | `yue`             | Sino-Tibetan          | Chinese                     | Cantonese              |
| `sit`       | `cmn`             | Sino-Tibetan          | Chinese                     | Mandarin               |
| `zls`       | `hr`              | South Slavic          | Croatian                    |                        |
| `zls`       | `cs`              | South Slavic          | Czech                       |                        |
| `gmq`       | `da`              | North Germanic        | Danish                      |                        |
| `gmw`       | `nl`              | West Germanic         | Dutch                       |                        |
| `gmw`       | `en-US`           | West Germanic         | English                     | American               |
| `gmw`       | `en`              | West Germanic         | English                     | British                |
| `gmw`       | `en-029`          | West Germanic         | English                     | Caribbean              |
| `gmw`       | `en-GB-x-gbclan`  | West Germanic         | English                     | Lancastrian            |
| `gmw`       | `en-GB-x-rp`      | West Germanic         | English                     | Received Pronunciation |
| `gmw`       | `en-GB-scotland`  | West Germanic         | English                     | Scottish               |
| `gmw`       | `en-GB-x-gbcwnd`  | West Germanic         | English                     | West Midlands          |
| `urj`       | `et`              | Uralic                | Estonian                    |                        |
| `ira`       | `fa`              | Iranian               | Farsi/Persian<sup>[1]</sup> |                        |
| `ira`       | `fa-en-us`        | Iranian               | Farsi/Persian<sup>[2]</sup> |                        |
| `ira`       | `fa-Latin`        | Iranian               | Farsi/Persian<sup>[3]</sup> |                        |
| `urj`       | `fi`              | Uralic                | Finnish                     |                        |
| `roa`       | `fr`              | Romance               | French                      | France                 |
| `roa`       | `fr-BE`           | Romance               | French                      | Belgian                |
| `cel`       | `ga`              | Celtic                | Gaelic                      | Irish                  |
| `cel`       | `gd`              | Celtic                | Gaelic                      | Scottish               |
| `ccs`       | `ka`              | South Caucasian       | Georgian                    |                        |
| `gmw`       | `de`              | West Germanic         | German                      |                        |
| `grk`       | `grc`             | Greek                 | Greek                       | Ancient                |
| `grk`       | `el`              | Greek                 | Greek                       | Modern                 |
| `esx`       | `kl`              | Eskimo-Aleut          | Greenlandic                 |                        |
| `sai`       | `gn`              | South American Indian | Guarani                     |                        |
| `inc`       | `gu`              | Indic                 | Gujarati                    |                        |
| `inc`       | `hi`              | Indic                 | Hindi                       |                        |
| `urj`       | `hu`              | Uralic                | Hungarian                   |                        |
| `gmq`       | `is`              | North Germanic        | Icelandic                   |                        |
| `poz`       | `in`              | Malayo-Polynesian     | Indonesian                  |                        |
| `art`       | `ia`              | Constructed           | Interlingua                 |                        |
| `roa`       | `it`              | Romance               | Italian                     |                        |
| `dra`       | `kn`              | Dravidian             | Kannada                     |                        |
|             | `ko`              |                       | Korean                      |                        |
| `ira`       | `ku`              | Iranian               | Kurdish                     |                        |
| `trk`       | `ky`              | Turkic                | Kyrgyz                      |                        |
| `itc`       | `la`              | Italic                | Latin                       |                        |
| `bat`       | `lv`              | Baltic                | Latvian                     |                        |
| `art`       | `lfn`             | Constructed           | Lingua Franca Nova          |                        |
| `bat`       | `lt`              | Baltic                | Lithuanian                  |                        |
| `art`       | `jbo`             | Constructed           | Lojban                      |                        |
| `zls`       | `mk`              | South Slavic          | Macedonian                  |                        |
| `poz`       | `ms`              | Malayo-Polynesian     | Malay                       |                        |
| `dra`       | `ml`              | Dravidian             | Malayalam                   |                        |
| `sem`       | `mt`              | Semitic               | Maltese                     |                        |
| `inc`       | `mr`              | Indic                 | Marathi                     |                        |
| `azc`       | `nci`             | Uto-Aztecan           | Nahuatl                     | Classical              |
| `inc`       | `ne`              | Indic                 | Nepali                      |                        |
| `gmq`       | `no`; `nb`        | North Germanic        | Norwegian                   |                        |
| `inc`       | `or`              | Indic                 | Oriya                       |                        |
| `cus`       | `om`              | Cushitic              | Oromo                       |                        |
| `roa`       | `pap`             | Romance               | Papiamento                  |                        |
| `zls`       | `pl`              | South Slavic          | Polish                      |                        |
| `roa`       | `pt-BR`           | Romance               | Portuguese                  | Brazillian             |
| `roa`       | `pt`              | Romance               | Portuguese                  | Portugal               |
| `inc`       | `pa`              | Indic                 | Punjabi                     |                        |
| `roa`       | `ro`              | Romance               | Romanian                    |                        |
| `zls`       | `ru`              | South Slavic          | Russian                     |                        |
| `zls`       | `sr`              | South Slavic          | Serbian                     |                        |
| `bnt`       | `tn`              | Bantu                 | Setswana                    |                        |
| `inc`       | `si`              | Indic                 | Sinhala                     |                        |
| `zls`       | `sk`              | South Slavic          | Slovak                      |                        |
| `zls`       | `sl`              | South Slavic          | Slovenian                   |                        |
| `roa`       | `es`              | Romance               | Spanish                     | Spain                  |
| `roa`       | `es-419`          | Romance               | Spanish                     | Latin American         |
| `bnt`       | `sw`              | Bantu                 | Swahili                     |                        |
| `gmq`       | `sv`              | North Germanic        | Swedish                     |                        |
| `dra`       | `ta`              | Dravidian             | Tamil                       |                        |
| `trk`       | `tt`              | Turkic                | Tatar                       |                        |
| `dra`       | `te`              | Dravidian             | Telugu                      |                        |
| `trk`       | `tr`              | Turkic                | Turkish                     |                        |
| `inc`       | `ur`              | Indic                 | Urdu                        |                        |
| `aav`       | `vi-VN-x-central` | Austroasiatic         | Vietnamese                  | Central Vietnam        |
| `aav`       | `vi`              | Austroasiatic         | Vietnamese                  | Northern Vietnam       |
| `aav`       | `vi-VN-x-south`   | Austroasiatic         | Vietnamese                  | Southern Vietnam       |
| `cel`       | `cy`              | Celtic                | Welsh                       |                        |

[1] Includes support for reading British English text.

[2] Includes support for reading American English text.

[3] Farsi/Persian written using English (Latin) characters.
