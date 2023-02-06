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

[CPNL](https://blogs.cpnl.cat/dgava12/files/2012/12/Full-de-ruta_unitat21.pdf)

[Apunts del IES Olorda](http://iesolorda.cat/departaments/cat/fon_tot.pdf)


## Tests
Poso un [exemple](https://github.com/projecte-aina/espeak-ng/blob/dev-ca/tests.tsv) de com ho pensava fer.

tsv amb: segment de text    transcripció    descripció  comentaris

a comentaris: "ok" vol dir implementat. "consultar" vol dir que s'ha de mirar la bibliografia.

PENDENTS:
* d aproximant quan no toca (calda, sap donar, )
* accent 1a pers plural pres ind
* com és la pronúncia de la d de "compres draps"?
* repassar les africades
* què passa amb la t de molt?
* tònica imperatius (irromp, corromp)
* mirar les u

## Dubtes
* fem geminades?
* com volem que soni la v?
* què fem amb i + vocal (camió, Núria, hi ha, etc.)
* porta uniforme
* molt espès
* els xàfegs, vas xiular; els jocs, has jugat; els rics, vas riure
