# Fork de desenvolupament de noves dades 
# per l'espeak en català

## Instal·lació
1) clonar el repo

2) instal·lar

Per instal·lar les dependències:
```
sudo apt-get install make autoconf automake libtool pkg-config
sudo apt-get install gcc libsonic-dev ronn kramdown
sudo apt-get install libpcaudio-dev
```

i després per fer el build:
```
$ ./autogen.sh 
$ ./configure --prefix=/usr
$ make
$ sudo make LIBDIR=/usr/lib/x86_64-linux-gnu install
``` 

## Funcionament
```
src/espeak-ng "Bon dia" --ipa -v ca
```

Per fer que no parli
```
src/espeak-ng "Bon dia" --ipa -v ca -q
```

## Editar
### Regles
Els diccionaris es troben a [dictsource](https://github.com/projecte-aina/espeak-ng/tree/master/dictsource).

Descripció dels diccionaris: [dictionary.md](https://github.com/espeak-ng/espeak-ng/blob/master/docs/dictionary.md).

1) Editar

2) compilar les regles i diccionaris
```
espeak-ng/dictsource$ ../src/espeak-ng --compile=ca
```

3) Provar per debug
```
espeak-ng$ src/espeak-ng -vca -X "text"
```

### Fonemes i al·lòfons
La definició dels fonemes es troba al directori [phsource/](https://github.com/projecte-aina/espeak-ng/tree/dev-ca/phsource).


- [instruccions](https://github.com/projecte-aina/espeak-ng/blob/dev-ca/docs/phontab.md#phoneme-definitions)
- [característiques dels sons](https://github.com/projecte-aina/espeak-ng/blob/dev-ca/docs/phonemes.md#consonants)


1) editar
Mirar els fitxers phonemes, ph_base2 i ph_catalan

2) compilar els fonemes
```
$ ../src/espeak-ng --compile-phonemes
```

3) compilar les regles i diccionaris
```
espeak-ng/dictsource$ ../src/espeak-ng --compile=ca
```

4) provar
```
src/espeak-ng "Bon dia" --ipa -v ca
```
## Documents
[Estàndard oral IEC](https://publicacions.iec.cat/repository/pdf/00000039/00000072.pdf)

[Aplicació al català dels principis de transcripció de l'Associació Fonètica Internacional](https://publicacions.iec.cat/repository/pdf/00000041/00000087.pdf)

[Requeriments SC](https://www.softcatala.org/wiki/Usuari:Jmas/TTS_Requeriments)

[Mots amb excepcions](https://ca.wiktionary.org/wiki/Categoria:Mots_en_catal%C3%A0_per_caracter%C3%ADstiques_fon%C3%A8tiques)

[Taules de sons](http://www.ub.edu/sonscatala/ca/central)

[La fonologia del català](http://diposit.ub.edu/dspace/bitstream/2445/67057/1/Fonologia_catalana.pdf)

[Fonètica del català](https://www.auladecatala.com/fonetica-i-ortografia/) (Aula de català)
[CPNL](https://blogs.cpnl.cat/dgava12/files/2012/12/Full-de-ruta_unitat21.pdf)

[Apunts del IES Olorda](http://iesolorda.cat/departaments/cat/fon_tot.pdf)

## Altres sistemes de transcripció
[Segre](https://nlp.lsi.upc.edu/freeling/demo/segre.php)

[TransText](https://sites.google.com/site/juanmariagarrido/research/resources/tools/transtext)

## Tests


