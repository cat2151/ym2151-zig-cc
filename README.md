# ym2151-zig-cc

A project to build a 440Hz 3-second audio rendering test using Nuked-OPM with zig cc.

## Current Status
### Pending Thoughts and Goals (to avoid cluttering issues)
- Challenges and goals are ill-defined.
  - Option 1
    - Aim to achieve pitch changes synchronized with mouse movement, similar to `oscsync`.
  - Option 2
    - As an implementation hint for `cat-play-mml`,
      - Aim to be able to read and play cycle-based register write logs (the simplest logs for the read side to process).
  - Decision?
    - Prioritize Option 2 first, as it has lower real-time requirements and is easier to make steady progress on.

## Out of Scope

This project prioritizes code simplicity, so features that would complicate the code, such as the following, are out of scope:

- Reproduction of clock and timing as if played on an X680x0
- Reproduction of existing music
- Advanced real-time performance

## Overview

This project is a C language program that renders a 440Hz tone for 3 seconds using Nuked-OPM (a Yamaha YM2151 emulator) and verifies that the output buffer is not silent. It uses `zig cc` for building.

## Features

- ✅ Accurate YM2151 emulation using the Nuked-OPM library
- ✅ Proper initialization using `OPM_SetIC` (to avoid silence)
- ✅ 100ms delay for initialization, 10ms delay after each register write
- ✅ Cross-platform build using `zig cc` (Linux, Windows)
- ✅ Automated testing with GitHub Actions
- ✅ Non-silent buffer verification
- ✅ WAV file output functionality (`src/phase2/`)
- ✅ Real-time audio playback functionality (`src/phase3/`)

## Quickstart

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

For detailed build instructions and environment setup, refer to [BUILD.md](BUILD.md).

## Example Test Result

```
✅ SUCCESS: Buffer is NON-SILENT!
The audio buffer contains audio data.
  Total samples: 264600
  Non-zero samples: 264600 (100.00%)
  Max absolute value: 8080
```

## WAV File Output (Phase2)

A program to output a 440Hz tone as a 3-second WAV file is implemented in `src/phase2/`.

```bash
# Build
python3 build.py build-phase2-gcc

# Run (generates output.wav)
./wav_output

# Run with custom filename
./wav_output my_sound.wav
```

For more details, refer to [src/phase2/README.md](src/phase2/README.md).

## Real-time Audio Playback (Phase3)

A program to play a 440Hz tone in real-time for 3 seconds is implemented in `src/phase3/`. It uses MiniAudio for data_callback-based real-time audio generation.

```bash
# Build
python3 build.py build-phase3-gcc

# Run (audio will play from speakers)
./real_time_audio
```

For more details, refer to [src/phase3/README.md](src/phase3/README.md).

## Documentation

- [BUILD.md](BUILD.md) - Detailed build instructions and troubleshooting
- [src/phase2/README.md](src/phase2/README.md) - Description of the WAV output program
- [src/phase3/README.md](src/phase3/README.md) - Description of the real-time audio playback program
- [.github/workflows/test.yml](.github/workflows/test.yml) - CI/CD configuration

## License

- This project: MIT License
- Nuked-OPM: LGPL 2.1
- MiniAudio: Public Domain OR MIT-0 (dual license)