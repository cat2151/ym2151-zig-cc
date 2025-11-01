# Phase4: Music Sequence Player

Phase4 implements a BPM120 music sequence player that plays 4 quarter notes (MIDI notes 60, 64, 67, 71) corresponding to C4, E4, G4, and B4.

## Features

- **2-Pass Processing**: 
  - Pass 1: Musical events without delays
  - Pass 2: Register events with hardware delays added
- **JSON Output**: Both passes are saved as JSON files for testing and verification
- **Real-time Playback**: Uses MiniAudio for real-time audio output
- **WAV File Output**: Simultaneously saves to WAV file during playback
- **Internal Sample Rate**: ~55.9 kHz (OPM clock / 64)
- **Precise Timing**: Uses absolute sample time to avoid delta time accumulation errors

## Building

```bash
# Build for Linux with gcc
python3 build.py build-phase4-gcc

# Build for Linux with zig cc
python3 build.py build-phase4

# Cross-compile for Windows
python3 build.py build-phase4-windows
```

## Running

```bash
# Run with default output filename (phase4_output.wav)
./phase4_player

# Run with custom output filename
./phase4_player my_music.wav
```

## Output Files

The program generates three files:

1. **phase4_pass1.json**: Musical events without delays
   - Shows the pure musical intent
   - Time values represent musical event timing

2. **phase4_pass2.json**: Register events with hardware delays
   - Shows actual register write timing
   - Includes 2-sample delays between register writes
   - Used for actual playback

3. **phase4_output.wav** (or custom name): WAV audio file
   - 16-bit stereo
   - Sample rate: ~55.9 kHz (internal rate)
   - Duration: ~3 seconds

## Implementation Details

### BPM 120 Timing

- BPM: 120
- Beats per second: 2.0
- Quarter note duration: 0.5 seconds
- Quarter note samples: 27,965 samples at ~55.9 kHz

### MIDI to YM2151 Mapping

The program uses a lookup table to convert MIDI note numbers to YM2151 KC/KF values:

| MIDI Note | Note Name | Frequency | KC Value | KF Value |
|-----------|-----------|-----------|----------|----------|
| 60        | C4        | 261.63 Hz | 0x40     | 0x00     |
| 64        | E4        | 329.63 Hz | 0x45     | 0x00     |
| 67        | G4        | 392.00 Hz | 0x48     | 0x00     |
| 71        | B4        | 493.88 Hz | 0x4D     | 0x00     |

### 2-Pass Processing

**Pass 1 (Musical):**
- Generates events at musical time points
- No consideration for hardware delays
- Easy to verify musical correctness

**Pass 2 (Hardware):**
- Adds 2-sample delays between consecutive register writes
- Events at the same musical time are spread out
- Ready for hardware playback

### Register Write Delay

Each register write requires a delay of 128 OPM clock cycles, which equals 2 samples at the internal sample rate:

- Register write delay: 128 cycles
- Cycles per sample: 64
- Delay in samples: 128 / 64 = 2 samples

## Architecture

```
Musical Intent (Pass 1)
    ↓
Add Hardware Delays (Pass 2)
    ↓
Real-time Playback Engine
    ├─→ Audio Device (via MiniAudio)
    └─→ WAV Buffer (for file output)
```

## Testing

The JSON files can be used to verify correctness:

```bash
# View pass1 events (musical intent)
cat phase4_pass1.json | python3 -m json.tool

# View pass2 events (with delays)
cat phase4_pass2.json | python3 -m json.tool

# Compare event counts
jq '.event_count' phase4_pass1.json phase4_pass2.json
```

## Future Enhancements

Possible improvements:
- Support for more complex rhythms
- Multiple channels/voices
- Velocity control
- Fine-tuning with KF values
- Reading from external music files (MML, MIDI, etc.)
