# Contribution Guide

- [Simple steps for your feedback](#simple-steps-for-your-feedback)
- [Steps for your contribution](#steps-for-your-contribution)

## Simple steps for your feedback

<a class="anchor" id="simplestep1">1. As eSpeak NG is open source software, you can get it working from many different sources, some of which may provide quite
old version. Before registering new issue, test that issue can be reproduced with latest development version at
[eSpeak NG online](https://odo.lv/Espeak) site.

<a class="anchor" id="simlestep2">2. Review known [eSpeak NG issues](https://github.com/espeak-ng/espeak-ng/issues), particularly about
[languages](https://github.com/espeak-ng/espeak-ng/labels/languages%2Fpronunciation) and add your comments on existing issues.

<a class="anchor" id="simlestep3">3. Look at and subscribe to [eSpeakNG mailing list](https://groups.io/g/espeak-ng)
to view and discuss other related topics.

<a class="anchor" id="simlestep4">4. If you decide to create new issue, look at guide [here](https://help.github.com/en/articles/creating-an-issue) and provide following information:

1. example of text, which demonstrates and allows to reproduce spelling error;
2. explanation, why produced spelling is wrong and how it should be corrected.

<a class="anchor" id="simlestep5">5. Close issue when it is resolved.

## Steps for your contribution
<a class="anchor" id="step1">1. Get familiar with [additive speech synthesis](https://en.wikipedia.org/wiki/Speech_synthesis#Formant_synthesis).

<a class="anchor" id="step2">2. Create fork of [espeak-ng](https://github.com/espeak-ng/espeak-ng) repository as it is described [here](https://help.github.com/en/articles/fork-a-repo).

<a class="anchor" id="step3">3. [Clone](https://help.github.com/en/articles/cloning-a-repository) and set up your development environment [on your computer](building.md) , or clone it [online](https://odo.lv/EspeakGuide). For [some easy fixes](https://github.com/espeak-ng/espeak-ng/labels/good%20first%20issue) you may change files just through browser of yor forked github project. Make necessary changes, commit them and issue pull request as described in [step 9](#9).

<a class="anchor" id="step4">4. Get familiar with [text to phoneme translation](dictionary.md) and how to [add or improve language](add_language.md).

<a class="anchor" id="step5">5. Get familiar with [voice files](voices.md) and [phoneme tables](phontab.md) configuration files.

<a class="anchor" id="step6">6. Make your improvements (i.e. additions or modifications) and check your project with `make check` command. Fix issues if necessary.

<a class="anchor" id="step7">7. If you contribute to an existing language, find language maintainer in language configuration file located in `espeak-ng-data/lang` folder[<sup>1</sup>](#1) and ask him to review your changes. If you can't find language maintainer, you can review list of all contributors[<sup>2</sup>](#2) and contact them. You can also volunteer to become language maintainer just by updating [language configuration file](voices.md) and submitting your changes as it is described in [step 9](#9).

<a class="anchor" id="step8">8. If you contribute to the core of espeak-ng engine (i.e. source files of software), look at [eSpeak NG roadmap](https://github.com/espeak-ng/espeak-ng/wiki/eSpeak-NG-roadmap). In this case you just issue pull requests as described in [step 9](#9), to make changes easier for review.

Note that:

1. Pull request can have one or many commits, but it should have single problem solved, which should be properly described in commits and in description of the pull request.
2. `make check` on your local computer and remote checks of the pull request on the server must pass.
3. There shouldn't be merging conflicts indicated.
4. For new/extended functionality new automated tests should be created.
5. There shouldn't be trivial changes only like code indentation, fixed typos, language style changes etc.

<a class="anchor" id="step9">9. [Push your changes to GitHub](https://help.github.com/en/articles/pushing-commits-to-a-remote-repository) and create [pull request](https://help.github.com/en/articles/creating-a-pull-request). You can also send changed files by email to [eSpeakNG mailing list](https://groups.io/g/espeak-ng), but then inlusion of them may take more time.

<a class="anchor" id="step10">10. Wait for feedback of current espeak-ng maintainers[<sup>3</sup>](#3).

<a class="anchor" id="step11">11. When you are ready to take responsibility, ask [current maintainers](https://github.com/espeak-ng/espeak-ng/people) to get promoted as a maintainer of the project.

----

<a class="anchor" id="1"></a>1. You can get list of all language maintainers with following commands in your `espeak-ng` folder:

```
cd espeak-ng-data;grep -ri -A2 -B2 maintainer *
```

<a class="anchor" id="2"></a>2. You can get list of all contributors with following commands in your `espeak-ng` folder:

```
git log --pretty="%an %ae%n%cn %ce" | sort|uniq -c|sort -nr
```
<a class="anchor" id="3"></a>3. Currently eSpeak NG is maintained only by volunteers in their free time, therefore expected reaction time is one, two weeks.

