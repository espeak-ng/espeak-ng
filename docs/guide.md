# eSpeak NG user guide

- [Installation](#installation)
  - [Linux](#linux)
  - [Windows](#windows)
- Command line usage
  - [espeak-ng Command Line](../src/espeak-ng.1.ronn)
  - [speak-ng Command Line](../src/speak-ng.1.ronn)
- Library usage
  - [Integration guide](integration.md)
- [Error solutions](#error-solutions)
  - [Problems with pcaudiolib](#problems-with-pcaudiolib)
  - [Problems with pulseaudio](#problems-with-pulseaudio)
- [Full list of documents](index.md)

## Installation
### Linux

To install precompiled package of eSpeak NG on Linux, use standard package manager of your
distribution. (Probably is very high, eSpeak NG will be included in package repository).

E.g. for Debian-like distribution (e.g. Ubuntu, Mint, etc.) execute command:

    sudo apt-get install espeak-ng

For RedHat-like distribution (e.g. CentOS, Fedora, etc.) execute command:

    sudo yum install espeak-ng
    
For ArchLinux-like distribution (e.g. SteamOS, EndeavourOS, etc.) execute command:

    sudo pacman -S espeak-ng

### Windows

To install precompiled binaries of eSpeak NG on Windows:

1. open (https://github.com/espeak-ng/espeak-ng/releases) and click on **Latest release** and then appropriate *.msi file, e.g. **espeak-ng-20191129-b702b03-x64.msi**

2. Execute downloaded installer package.

## Error solutions

## Problems with pcaudiolib

if `espeak-ng` is compiled and installed without [pcaudiolib](https://github.com/espeak-ng/pcaudiolib) compiled and installed before, then calling it simply

    espeak-ng hello

doesn't produce any sound.
Then usual workaround with sending data standard output and piping it to `aplay` (Lubuntu) or `paplay` (Ubuntu) works:

    espeak-ng hello --stdout|aplay

But, when `pcaudiolib` is compiled and installed and espeak-ng is reconfigured, recompiled and reinstalled again, simple call works properly.

If espeak-ng is built from source code, ensure that `libpulse-dev` package is installed, or [pcaudiolib](https://github.com/espeak-ng/pcaudiolib) built and installed and, when do `./configure` command, check that status says:

    PulseAudio support:            yes

Then recompile and install espeak-ng again.


## Problems with pulseaudio

If Linux distribution doesn't support pulseaudio sound system (or it doesn't work):

1. Remove _pulseaudio_ package install _alsa_:

       sudo apt purge pulseaudio
       sudo apt autoremove
       sudo apt install --reinstall alsa
2. Reboot

       sudo reboot
3. Recompile espeak-ng with ` --with-pulseaudio=no` support:

       ./autogen.sh
       ./configure --prefix=/usr --with-pulseaudio=no
       make -B
       sudo make install

