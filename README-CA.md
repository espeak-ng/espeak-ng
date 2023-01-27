# Fork de desenvolupament de noves dades per l'Espeak en català

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
Els diccionaris es troben a [dictsource](https://github.com/projecte-aina/espeak-ng/tree/master/dictsource).

Descripció dels diccionaris: [dictionary.md](https://github.com/espeak-ng/espeak-ng/blob/master/docs/dictionary.md).

1) Editar

2) compilar els diccionaris
```
espeak-ng/dictsource$ ../src/espeak-ng --compile=ca
```

3) Provar per debug
```
espeak-ng$ src/espeak-ng -vca -X "text"
```


## Documents
[Estàndard oral IEC](https://publicacions.iec.cat/repository/pdf/00000039/00000072.pdf)

[Requeriments SC](https://www.softcatala.org/wiki/Usuari:Jmas/TTS_Requeriments)

[Mots amb excepcions](https://ca.wiktionary.org/wiki/Categoria:Mots_en_catal%C3%A0_per_caracter%C3%ADstiques_fon%C3%A8tiques)

[Taules de sons](http://www.ub.edu/sonscatala/ca/central)


## Tests
Quan el tingui fet pujaré un fitxer de tests
