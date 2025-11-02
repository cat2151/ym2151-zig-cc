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

This project is an experimental C language program that plays simple sounds using Nuked-OPM (a Yamaha YM2151 emulator). It uses zig cc for building.

## Features

- ✅ Accurate YM2151 emulation using the Nuked-OPM library
- ✅ Windows build using zig cc
- ✅ WAV file output functionality (src/phase2/)
- ✅ Real-time audio playback functionality (src/phase3/)
- ✅ Simple sequencer functionality (src/phase4/)
- ✅ Provides an easy way to experience YM2151 sounds on Windows
- ✅ Simple code that may be easy to reference

## Build

```powershell
python build.py build-phase4 && ./player.exe
```

## Maintenance

- If it stops working, I intend to prioritize restoring it to a playable state.

## Future Prospects

- If there are obvious flaws in the code, comments, or documentation, I will consider making corrections.

## Out of Scope

This project prioritizes code simplicity, so the following features that would complicate the code are out of scope:

- Any feature additions that complicate the code, such as improving versatility for direct library referencing.
- Interactive performance
- Reproduction of clock and timing as if played on an X680x0
- Reproduction of existing songs

## License

- This project: MIT License
- Nuked-OPM: LGPL 2.1
- MiniAudio: Public Domain OR MIT-0 (dual license)

※ The English README.md is automatically generated from README.ja.md using Gemini's translation via GitHub Actions.