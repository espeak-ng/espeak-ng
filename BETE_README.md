# Test IPA Bété pour eSpeak-ng

## 📋 Verset biblique de test (Psaume 150:1)

**Texte Bété :** `A ghlimanɩ -Lagɔ, na kpö yi -Lagɔ ghlimanɩ!`

**Transcription IPA :** `[a ʔɣlimanɩ -laɡɔ, na ʔkpö ji -laɡɔ ʔɣlimanɩ!]`

**Traduction FR :** Louez l'Éternel ! Mon âme, loue l'Éternel !

**Traduction EN :** Praise the Lord! Praise the Lord, O my soul!

---

## 🚀 Comment utiliser ces fichiers

### **Étape 1 : Compiler eSpeak-ng avec support Bété**

```bash
# Dans le répertoire racine du projet
cmake -Bbuild -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target data
cmake --build build
```

### **Étape 2 : Lancer le test Bété**

```bash
cd build
ctest -R language-bete --output-on-failure
```

Ou directement :

```bash
ESPEAK_DATA_PATH=$(pwd) ../tests/language-bete.test
```

### **Étape 3 : Tester votre texte IPA Bété**

```bash
ESPEAK_DATA_PATH=$(pwd) ./src/espeak-ng -xq -v bof "A ghlimanɩ -Lagɔ, na kpö yi -Lagɔ ghlimanɩ!"
```

---

## 📁 Fichiers créés

| Fichier | Description |
|---------|-------------|
| `espeak-ng-data/lang/bnt/bof` | Configuration de la voix Bété (nom, code langue, statut) |
| `dictsource/bof_rules` | Règles de conversion lettres → phonèmes |
| `dictsource/bof_list` | Exceptions et mots spéciaux |
| `tests/language-bete.test` | Test du verset biblique |

---

## 📝 Phonèmes Bété reconnus

| Lettre(s) | Phonème IPA | Exemple |
|-----------|-----------|---------|
| `a` | [a] | A (Praise) |
| `gh` | [ɣ] | ghlimanɩ (Lord) |
| `'` | [ʔ] | 'ghlimanɩ (Lord - glottal stop) |
| `kp` | [kp] | kpö (beat) |
| `j` | [dZ] ou [j] | ji (Praise) |
| `-` | (séparateur syllabique) | -Lagɔ |

---

## 🔧 Comment améliorer le support Bété

Pour un support complet, vous devez :

1. **Étendre `bof_rules`** avec plus de règles de conversion lettres → phonèmes
2. **Ajouter à `bof_list`** tous les mots spéciaux/exceptions
3. **Créer des phonèmes personnalisés** dans `phsource/ph_bof` si les phonèmes IPA ne suffisent pas
4. **Tester d'autres textes Bété** et ajuster les règles

---

## ✅ Vérification

Pour vérifier que tout fonctionne :

```bash
# 1. Vérifier les fichiers existent
ls espeak-ng-data/lang/bnt/bof
ls dictsource/bof_*
ls tests/language-bete.test

# 2. Compiler
cmake -Bbuild -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target data

# 3. Lancer le test
cd build
ctest -R language-bete --output-on-failure
```

---

## 🎯 Prochaines étapes

1. **Enrichir les règles** : Vous pouvez ajouter d'autres règles phonétiques Bété dans `bof_rules`
2. **Valider l'output** : Comparer la sortie phonétique avec votre transcription IPA attendue
3. **Contribuer** : Ces fichiers peuvent être soumis au projet eSpeak-ng !

---

**Créé pour l'étude de la langue Bété de Daloa** 🇨🇮
