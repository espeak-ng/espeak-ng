Nano Syntax Files
============================

These are the steps to add syntax highlighting for nano editor:

1. Copy all `nanorc` files in this folder into `~/.nano` hidden folder in your home directory.

```bash
cp nano/*.nanorc  ~/.nano
```

2. Add these lines to `~/.nanorc` hidden file in your home directory:

```
include ~/.nano/espeak-list.nanorc
include ~/.nano/espeak-rules.nanorc
```

