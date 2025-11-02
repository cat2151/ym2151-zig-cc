# ym2151-zig-cc

A project to build a simple sequencer using Nuked-OPM with zig cc

<p align="left">
  <a href="README.ja.md"><img src="https://img.shields.io/badge/🇯🇵-Japanese-red.svg" alt="Japanese"></a>
  <a href="README.md"><img src="https://img.shields.io/badge/🇺🇸-English-blue.svg" alt="English"></a>
</p>

## Quick Links
| Item | Link |
|------|--------|
| 📊 Development Status | [generated-docs/development-status](generated-docs/development-status.md) |

## Overview

This project is an experimental C language program that uses Nuked-OPM (Yamaha YM2151 emulator) to play simple sounds. It uses zig cc for building.

## Features

- ✅ Accurate YM2151 emulation using the Nuked-OPM library
- ✅ Windows build using zig cc
- ✅ WAV file output functionality (src/phase2/)
- ✅ Real-time audio playback functionality (src/phase3/)
- ✅ Simple sequencer functionality (src/phase4/)
- ✅ Provides an easy experience to play YM2151 sounds on Windows
- ✅ Simple codebase that may serve as a good reference

## Build

```powershell
python build.py build-phase4 && ./player.exe
```

## Maintenance

- If playback stops working, I intend to prioritize restoring it to a functional state.

## Future Outlook

- If there are any clear deficiencies in the code, comments, or documentation, I will consider corrections.

## Out of Scope

This project prioritizes code simplicity. Therefore, features that would complicate the code, such as the following, are out of scope:

- Any feature additions that increase code complexity, such as improving versatility for direct use of source files as a library.
- Interactive performance
- Reproduction of clock and timing as if played on X680x0
- Reproduction of existing songs

## License

- This project: MIT License
- Nuked-OPM: LGPL 2.1
- MiniAudio: Public Domain OR MIT-0 (dual license)

※The English README.md is automatically generated from README.ja.md using Gemini's translation via GitHub Actions.