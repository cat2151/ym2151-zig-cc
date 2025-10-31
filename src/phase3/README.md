# Phase3: Real-Time Audio Playback with MiniAudio

This directory contains the real-time audio playback version of the Nuked-OPM 440Hz test program using MiniAudio.

## Overview

The `real_time_audio.c` program plays a 3-second 440Hz (A4) tone in real-time using the YM2151 (OPM) chip emulator with MiniAudio's data callback mechanism.

## Features

- Real-time audio playback using MiniAudio
- Generates audio on-the-fly using data_callback
- Uses the same Nuked-OPM configuration as phase2
- Outputs 440Hz A4 tone for 3 seconds
- Cross-platform support (Linux, Windows, macOS)
- 44.1kHz, 16-bit stereo audio output

## Key Differences from Phase2

Phase2 (WAV Output):
- Pre-renders all audio to a buffer
- Writes buffer to a WAV file
- No real-time constraints

Phase3 (Real-Time Audio):
- Generates audio samples in real-time via callback
- Plays directly through audio device
- Must maintain real-time performance

## Building

### With build.py (Recommended)

#### Current Platform (Linux/Windows)
```bash
python3 build.py build-phase3
```

#### With gcc (Linux only)
```bash
python3 build.py build-phase3-gcc
```

#### Windows Executable (cross-compile from Linux)
```bash
python3 build.py build-phase3-windows
```

### Manual Build

#### Linux with gcc
```bash
gcc -o real_time_audio src/phase3/real_time_audio.c opm.c -lm -lpthread -ldl
```

#### Linux with zig cc
```bash
zig cc -o real_time_audio src/phase3/real_time_audio.c opm.c -lm -lpthread -ldl
```

#### Windows with zig cc
```cmd
zig cc -o real_time_audio.exe src/phase3/real_time_audio.c opm.c -lm
```

#### Cross-compile for Windows from Linux
```bash
zig cc -target x86_64-windows -o real_time_audio.exe src/phase3/real_time_audio.c opm.c -lm
```

## Usage

### Linux/macOS
```bash
./real_time_audio
```

### Windows
```cmd
real_time_audio.exe
```

The program will play a 440Hz tone through your default audio output device for 3 seconds.

## Example Output

```
Nuked-OPM Real-Time Audio Program (MiniAudio)
==============================================

Configuring OPM for 440Hz tone...
  Writing registers...
  Key ON channel 0, all operators...
  Configuration complete.

Initializing MiniAudio device...
✅ MiniAudio device initialized successfully
   Sample rate: 44100 Hz
   Channels: 2 (stereo)
   Format: 16-bit signed integer

Starting playback...
Playing 440Hz tone for 3 seconds...

✅ Playback completed!
   Total samples played: 132300

✅ SUCCESS!
Real-time audio playback completed successfully.
```

## Technical Details

### MiniAudio Integration

MiniAudio is a single-header audio playback library that provides:
- Cross-platform audio device access
- Simple callback-based API
- No external dependencies beyond system libraries

The program uses MiniAudio's `data_callback` function which is called by the audio thread when the device needs more audio data. This callback:
1. Generates audio samples on-demand using the OPM chip
2. Clocks the chip 64 times per audio sample
3. Converts 32-bit samples to 16-bit for audio output
4. Tracks playback progress and stops after 3 seconds

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
- Audio is generated in real-time as the callback is invoked
- Samples are converted from 32-bit to 16-bit on-the-fly
- Playback stops automatically after 3 seconds

## Troubleshooting

### No Sound

If you don't hear any sound:

1. Check that your audio device is working properly
2. Verify the program reports "Playback completed!"
3. Ensure the volume is not muted
4. Try running with different audio backends (MiniAudio auto-selects)

### Audio Glitches

If you hear glitches or stuttering:

1. The CPU may be too slow for real-time processing
2. Try closing other applications
3. Check system audio settings

### Build Errors

If you encounter build errors:

1. Ensure `opm.c` and `opm.h` are in the repository root
2. For Linux, ensure pthread and dl libraries are available
3. Verify gcc or zig is installed and in PATH
4. Check that miniaudio.h is present in src/phase3/

## Dependencies

### Runtime Dependencies

- **Linux**: ALSA, PulseAudio, or JACK (MiniAudio auto-detects)
- **Windows**: Windows Audio Session API (WASAPI)
- **macOS**: Core Audio

### Build Dependencies

- **Linux**: pthread, dl (libpthread, libdl)
- **Windows**: None (uses Windows APIs)

## License

- This program: MIT License
- Nuked-OPM: LGPL 2.1
- MiniAudio: Public Domain OR MIT-0 (dual license)
