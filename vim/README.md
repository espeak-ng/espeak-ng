Vim Syntax Files
============================

These are the steps to add syntax highlighting for vim editor:

1. Copy all subfolders in this folder into `~/.vim` hidden folder in your home directory.

```bash
cp -a vim/*(ftdetect|registry|syntax) ~/.vim
```

2. Add this line to `~/.vimrc` hidden file in your home directory:

```
syntax on
```

