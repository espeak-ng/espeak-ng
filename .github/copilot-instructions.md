# GitHub Copilot Instructions for eSpeak NG

## Project Overview

eSpeak NG is a compact open source software text-to-speech synthesizer that supports over 100 languages and accents. It uses formant synthesis to provide speech synthesis in a small package size. The project is written in C (C99 standard) and uses autotools for building.

## Key Project Structure

- `src/libespeak-ng/` - Core speech synthesis library (C source files)
- `src/include/` - Public and private headers
- `dictsource/` - Dictionary source files for all languages (text to phoneme rules and word lists)
- `phsource/` - Phoneme source files (phoneme definitions, sound data, formant values)
- `espeak-ng-data/` - Compiled data files (generated from dictsource and phsource)
- `tests/` - Automated test suite
- `docs/` - Project documentation

## Build System

- Uses autotools (autoconf, automake, libtool)
- Build workflow: `./autogen.sh` → `./configure` → `make`
- Always run `make check` to validate changes before committing
- The build system compiles dictionary and phoneme data as part of the build process

## Code Style

- Written in C99 standard C (some C++ for optional components)
- Tab-based indentation (use tabs, not spaces)
- Include copyright headers with GPL v3 license in new files
- Follow existing naming conventions (snake_case for functions and variables)
- Use `#ifdef __cplusplus` guards in headers for C++ compatibility

## Testing Requirements

- All changes must pass `make check` before submission
- Existing tests are in shell scripts (*.test) and C programs
- For new/extended functionality, create appropriate automated tests
- Test scripts follow a common pattern (see `tests/common` for utilities)
- Valgrind is used for memory checking in tests when available

## Language and Dictionary Files

### Dictionary Files (dictsource/)
- Format: `<language_code>_list` - word exceptions and pronunciations
- Format: `<language_code>_rules` - letter-to-phoneme translation rules
- Format: `<language_code>_emoji` - emoji pronunciations
- Each language may have a maintainer listed in its voice file

### Phoneme Files (phsource/)
- Phoneme definitions using specific syntax (see docs/phontab.md)
- Organized in subdirectories by phoneme table
- Reference WAV files for consonant sounds
- Reference formant data files for vowel sounds

## Contribution Guidelines

1. **Single Problem Per PR**: Each pull request should solve one specific problem
2. **Must Pass Tests**: Both local `make check` and CI checks must pass
3. **No Merge Conflicts**: Resolve any conflicts before review
4. **Meaningful Changes**: Avoid trivial changes (whitespace, typos only, code style only)
5. **Documentation**: Update relevant documentation for user-facing changes
6. **Commit Messages**: Write clear, descriptive commit messages

## Common Development Tasks

### Adding/Improving Language Support
1. Review `docs/add_language.md` for detailed process
2. Understand text-to-phoneme translation (see `docs/dictionary.md`)
3. Study phoneme tables (see `docs/phontab.md`) and voice files (see `docs/voices.md`)
4. Test with multiple text samples to validate pronunciation
5. Consider contacting the language maintainer if one exists

### Modifying Core Engine
1. Check the project roadmap for alignment
2. Ensure C99 compatibility
3. Test across multiple languages to avoid regressions
4. Document any API changes

### Working with Phonemes
- Phoneme names follow specific conventions (see `docs/phonemes.md`)
- IPA (International Phonetic Alphabet) names are automatically generated but can be overridden
- Use `length` instructions for vowels (typical: 140ms short, 200-300ms long)
- Use `WAV()` for unvoiced consonants, `FMT()` for vowels and voiced consonants

## Important Files to Reference

- `docs/contributing.md` - Complete contribution guide
- `docs/building.md` - Build instructions for all platforms
- `docs/dictionary.md` - Dictionary file format and rules
- `docs/phontab.md` - Phoneme definition syntax and instructions
- `docs/voices.md` - Voice configuration file format
- `docs/add_language.md` - Guide for adding new language support

## Special Considerations

- eSpeak NG is maintained by volunteers with limited time (expect 1-2 week response times)
- The project supports many platforms: Linux, Windows, Mac, Android, and more
- Some features like SAPI5 are not yet fully implemented
- The `espeakedit` program (for creating vowel files) is not part of this repository
- Cross-platform compatibility is important - test on multiple platforms when possible

## Git Workflow

- Create feature branches for new work
- Keep commits focused and atomic
- Write descriptive commit messages
- Reference issue numbers in commits when applicable
- Ensure the repository is clean (`make check` passes) before creating PR

## What NOT to Do

- Don't modify generated files in `espeak-ng-data/` directly (modify source files instead)
- Don't add unnecessary dependencies
- Don't make bulk formatting or style changes without functional improvements
- Don't remove or modify existing tests unless they're directly related to your changes
- Don't assume all tests need to pass if failures are pre-existing and unrelated to your work

## Resources

- Online testing: https://odo.lv/Espeak
- Mailing list: https://groups.io/g/espeak-ng
- Issue tracker: https://github.com/espeak-ng/espeak-ng/issues
- IPA reference: International Phonetic Alphabet standard
