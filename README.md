# ym2151-zig-cc

A project to build a simple sequencer utilizing Nuked-OPM with zig cc.

<p align="left">
  <a href="README.ja.md"><img src="https://img.shields.io/badge/🇯🇵-Japanese-red.svg" alt="Japanese"></a>
  <a href="README.md"><img src="https://img.shields.io/badge/🇺🇸-English-blue.svg" alt="English"></a>
</p>

## Quick Links
| Item | Link |
|------|--------|
| 📊 Development Status | [generated-docs/development-status](generated-docs/development-status.md) |

## Overview

This project is an experimental C language program that uses Nuked-OPM (Yamaha YM2151 emulator) to play simple sounds. It is built using zig cc.

## Features

- ✅ Accurate YM2151 emulation using the Nuked-OPM library
- ✅ Windows build using zig cc
- ✅ WAV file output (src/phase2/)
- ✅ Real-time audio playback (src/phase3/)
- ✅ Simple sequencer functionality (src/phase4/)
- ✅ Provides an easy way to experience YM2151 sounds on Windows
- ✅ Simple and easy-to-understand code for reference (perhaps)

## Out of Scope

This project prioritizes code simplicity, so features that would complicate the code are out of scope, such as:

- Reproduction of clock and timing as if played on an X680x0
- Reproduction of existing songs
- Interactive performance

## License

- This project: MIT License
- Nuked-OPM: LGPL 2.1
- MiniAudio: Public Domain OR MIT-0 (dual license)

※ The English README.md is automatically generated from README.ja.md by GitHub Actions using Gemini's translation.