let s:local_vim = expand('<sfile>:p:h') . '/vim'
if isdirectory(s:local_vim)
  execute 'setlocal runtimepath^=' . fnameescape(s:local_vim)

  runtime! ftdetect/**/*.vim

  if exists('&syntax') && &syntax == ''
    syntax enable
  endif
endif
