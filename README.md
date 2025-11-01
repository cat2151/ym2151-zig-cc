# ym2151-zig-cc

A project to build a 440Hz, 3-second audio rendering test using Nuked-OPM with zig cc.

## Status
### Deferred thoughts to avoid cluttering actual issues
- Goals are fuzzy.
  - Option 1
    - Aim to produce a 440Hz tone with a ±1Hz error.
  - Option 2
    - Consider it complete if it produces sound, regardless of pitch.
  - Option 3
    - Aim to change pitch in conjunction with mouse movement, similar to oscsync.
  - Option 4
    - As a hint for `cat-play-mml` implementation,
      - Aim to be able to read and play a log of register writes per cycle (the simplest log for the reader to process).
      - Prerequisite: Realization of either Option 1 or Option 4 is essential.
  - Option 5
    - Achieve internal 55KHz (or internal 62.5KHz with 4MHz clock),
    - Downsample to 48000Hz.
    - Consequently, Option 1 will also be achieved.
    - Method: Render the requested number of samples at internal 62.5KHz, then downsample.
      - The resampler has state, so there will be no failure at buffer boundaries.

## Overview

This project is a C program that uses Nuked-OPM (a Yamaha YM2151 emulator) to render a 440Hz tone for 3 seconds and verifies that the output buffer is not silent. It uses zig cc for building.

## Features

- ✅ Accurate YM2151 emulation using the Nuked-OPM library
- ✅ Proper initialization using OPM_SetIC (to avoid silence)
- ✅ 100ms delay for initialization, 10ms delay after each register write
- ✅ Cross-platform build using zig cc (Linux, Windows)
- ✅ Automated testing with GitHub Actions
- ✅ Non-silent buffer verification
- ✅ WAV file output functionality (src/phase2/)
- ✅ Real-time audio playback functionality (src/phase3/)

## Quick Start

### Linux

```bash
# Build using zig cc
zig cc -o test_opm test_opm.c opm.c -lm

# Run
./test_opm
```

### Windows

```cmd
zig cc -o test_opm.exe test_opm.c opm.c -lm
test_opm.exe
```

For detailed build instructions and environment setup, please refer to [BUILD.md](BUILD.md).

## Example Test Output

```
✅ SUCCESS: Buffer is NON-SILENT!
The audio buffer contains audio data.
  Total samples: 264600
  Non-zero samples: 264600 (100.00%)
  Max absolute value: 8080
```

## WAV File Output (Phase2)

A program that outputs a 440Hz tone as a 3-second WAV file is implemented in `src/phase2/`.

```bash
# Build
python3 build.py build-phase2-gcc

# Run (generates output.wav)
./wav_output

# Run with custom filename
./wav_output my_sound.wav
```

For more details, please refer to [src/phase2/README.md](src/phase2/README.md).

## Real-time Audio Playback (Phase3)

A program that plays a 440Hz tone in real-time for 3 seconds is implemented in `src/phase3/`. This is MiniAudio-based real-time audio generation using a data_callback.

```bash
# Build
python3 build.py build-phase3-gcc

# Run (sound will play from speakers)
./real_time_audio
```

For more details, please refer to [src/phase3/README.md](src/phase3/README.md).

## Documentation

- [BUILD.md](BUILD.md) - Detailed build instructions and troubleshooting
- [src/phase2/README.md](src/phase2/README.md) - Description of the WAV output program
- [src/phase3/README.md](src/phase3/README.md) - Description of the real-time audio playback program
- [.github/workflows/test.yml](.github/workflows/test.yml) - CI/CD settings

## License

- This project: MIT License
- Nuked-OPM: LGPL 2.1
- MiniAudio: Public Domain OR MIT-0 (dual license)