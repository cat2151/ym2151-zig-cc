# Phase2: WAV Output Version

This directory contains the WAV output version of the Nuked-OPM 440Hz test program.

## Overview

The `wav_output.c` program generates a 3-second WAV file containing a 440Hz (A4) tone produced by the YM2151 (OPM) chip emulator.

## Features

- Generates 44.1kHz, 16-bit stereo WAV files
- Uses the same Nuked-OPM configuration as the test program
- Outputs 440Hz A4 tone for 3 seconds
- Compatible with Windows and other platforms
- Validates audio buffer before writing WAV file

## Building

### With build.py (Recommended)

#### Current Platform (Linux/Windows)
```bash
python3 build.py build-phase2
```

#### With gcc (Linux only)
```bash
python3 build.py build-phase2-gcc
```

#### Windows Executable (cross-compile from Linux)
```bash
python3 build.py build-phase2-windows
```

### Manual Build

#### Linux with zig cc
```bash
zig cc -o wav_output src/phase2/wav_output.c opm.c -lm
```

#### Linux with gcc
```bash
gcc -o wav_output src/phase2/wav_output.c opm.c -lm
```

#### Windows with zig cc
```cmd
zig cc -o wav_output.exe src/phase2/wav_output.c opm.c -lm
```

#### Cross-compile for Windows from Linux
```bash
zig cc -target x86_64-windows -o wav_output.exe src/phase2/wav_output.c opm.c -lm
```

## Usage

### Default Output (output.wav)
```bash
./wav_output
```

### Custom Output Filename
```bash
./wav_output my_sound.wav
```

On Windows:
```cmd
wav_output.exe my_sound.wav
```

## Output Format

The generated WAV file has the following specifications:

- **Sample Rate**: 44,100 Hz
- **Bit Depth**: 16-bit
- **Channels**: 2 (Stereo)
- **Duration**: 3 seconds
- **File Size**: ~517 KB
- **Frequency**: 440 Hz (A4 note)

## Example Output

```
Nuked-OPM 440Hz WAV Output Program
===================================

Configuring OPM for 440Hz tone...
  Writing registers...
  Key ON channel 0, all operators...
  Configuration complete.

Rendering 3 seconds of audio...
  First non-zero sample at index 0: L=-7952 R=-7952

Analyzing rendered audio buffer...
  Total samples: 264600
  Non-zero samples: 264600 (100.00%)
  Max absolute value: 8080
  Average absolute value: 2333.82

✅ Audio buffer contains valid audio data.

Writing WAV file: output.wav
  Converting and writing audio data...
  WAV file written successfully!
  File size: 529244 bytes
  Duration: 3 seconds
  Sample rate: 44100 Hz
  Channels: 2 (stereo)
  Bits per sample: 16

✅ SUCCESS!
WAV file 'output.wav' created successfully.
```

## Technical Details

### OPM Configuration

The program configures the YM2151 (OPM) chip with the following settings:

- **Channel**: 0
- **Algorithm**: CON=7 (all operators as carriers)
- **Feedback**: FB=0 (no feedback)
- **Output**: Both L and R channels enabled
- **Key Code**: 0x4A (440Hz)
- **Operators**: All 4 operators configured, but only operator 0 at full volume

### Audio Rendering

- The chip is clocked 64 times per audio sample
- Audio is rendered as 32-bit samples and then converted to 16-bit
- Non-silent audio is validated before WAV file creation

## Troubleshooting

### No Sound in WAV File

If the generated WAV file is silent:

1. Check that the program reports "Audio buffer contains valid audio data"
2. Verify the OPM_SetIC initialization is working correctly
3. Ensure register values are being written properly

### Build Errors

If you encounter build errors:

1. Ensure `opm.c` and `opm.h` are in the parent directory (repository root)
2. Verify zig or gcc is installed and in PATH
3. Check that all required source files are present

## License

- This program: MIT License
- Nuked-OPM: LGPL 2.1
