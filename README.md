# ym2151-zig-cc

A project to build a simple sequencer utilizing Nuked-OPM with zig cc.

## Overview

This project is an experimental C language program that plays simple sounds using Nuked-OPM (Yamaha YM2151 emulator). zig cc is used for building.

## Features

- ✅ Accurate YM2151 emulation using the Nuked-OPM library
- ✅ Windows build using zig cc
- ✅ WAV file output functionality (src/phase2/)
- ✅ Real-time audio playback functionality (src/phase3/)
- ✅ Simple sequencer functionality (src/phase4/)
- ✅ Provides an easy experience to play YM2151 sounds on Windows
- ✅ Simple code that may be easy to use as a reference

## Out of Scope

This project prioritizes code simplicity; therefore, features that would complicate the code, such as the following, are out of scope:

- Reproduction of clock and timing as if played on an X680x0
- Reproduction of existing songs
- Interactive performance

## License

- This project: MIT License
- Nuked-OPM: LGPL 2.1
- MiniAudio: Public Domain OR MIT-0 (dual license)