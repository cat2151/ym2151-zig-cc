# ym2151-zig-cc

A project for building a simple sequencer leveraging Nuked-OPM with zig cc.

<p align="left">
  <a href="README.ja.md"><img src="https://img.shields.io/badge/🇯🇵-Japanese-red.svg" alt="Japanese"></a>
  <a href="README.md"><img src="https://img.shields.io/badge/🇺🇸-English-blue.svg" alt="English"></a>
</p>

## Quick Links
| Item | Link |
|------|--------|
| 📊 Development Status | [generated-docs/development-status](generated-docs/development-status.md) |

## Overview

This project is an experimental C language program that plays simple sounds using Nuked-OPM (a Yamaha YM2151 emulator). It uses zig cc for building.

## Features

- ✅ Accurate YM2151 emulation using the Nuked-OPM library
- ✅ Windows builds using zig cc
- ✅ WAV file output functionality (src/phase2/)
- ✅ Real-time audio playback functionality (src/phase3/)
- ✅ Simple sequencer functionality (src/phase4/)
- ✅ Provides an easy way to experience YM2151 sounds on Windows
- ✅ Simple and easy-to-reference code (perhaps)

## Build

```powershell
python build.py build-phase4 && ./player.exe
```

## Target Platforms

Windows

Linux is not officially tested. Voluntary Linux implementations and tests by contributors are tolerated from an overall project optimization perspective.

## Maintenance

- If the project stops producing sound, I intend to prioritize restoring it to a playable state as quickly as possible.

## Future Outlook

- If there are clear deficiencies in the code, comments, or documentation, I will consider making corrections.

## Out of Scope

To prioritize code simplicity, this project explicitly excludes the following features, which would complicate the codebase:

- Active Linux support by users or other multi-platform compatibility.
- Enhancements for general usability, such as directly utilizing source files as a library, or any other feature additions that would complicate the code.
- Interactive performance.
- Reproduction of clock and timing as if performed on an X680x0.
- Reproduction of existing songs.

## License

- This project: MIT License
- Nuked-OPM: LGPL 2.1
- MiniAudio: Public Domain OR MIT-0 (dual license)

*The English README.md is automatically generated from README.ja.md by GitHub Actions using Gemini's translation.*