# YM2151-Zig-CC Copilot Instructions

## Project Overview
This is a Yamaha YM2151 (OPM) FM synthesizer emulation project using the Nuked-OPM library. The project generates 440Hz test tones across three progressive phases: buffer validation, WAV file output, and real-time audio playback.

## Architecture & Core Components

### Core Emulation (`opm.c` / `opm.h`)
- **Nuked-OPM**: LGPL 2.1 licensed YM2151 emulator (version 0.9.2 beta)
- **Key API**: `OPM_Clock()`, `OPM_Write()`, `OPM_SetIC()`, `OPM_Reset()`
- **Critical pattern**: Use 12 cycles delays after each register write for proper initialization to avoid silent output

### Three-Phase Architecture
```
src/phase1/  → Buffer validation test (test_opm.c)
src/phase2/  → WAV file output (wav_output.c) 
src/phase3/  → Real-time audio (real_time_audio.c + miniaudio.h)
```

## Build System & Development Workflow

### Primary Build Tool: `build.py`
Use Python build script instead of manual compilation:
```bash
# Standard builds
python3 build.py                    # Current platform
python3 build.py build-phase2       # WAV output
python3 build.py build-phase3       # Real-time audio

# Cross-compilation
python3 build.py build-windows      # Cross-compile for Windows
```

### Compiler Requirements
- **Preferred**: `zig cc` for cross-platform builds
- **Alternative**: `gcc` (Linux only, use `build-*-gcc` commands)
- **Essential flags**: `-lm -fwrapv` (math library + integer overflow wrapping)
- **Phase3 additional**: `-lpthread -ldl` (for miniaudio on Linux)

## YM2151 Configuration Patterns

### Standard 440Hz Setup Sequence
1. Reset all channels (key off): `write_register_with_delay(chip, 0x08, ch, dummy_output)`
2. Configure channel settings:
   - `0x20 + channel`: RL_FB_CONNECT (0xC7 = both L/R, no feedback, all carriers)
   - `0x28 + channel`: KC key code (0x4A for 440Hz A4)
   - `0x30 + channel`: KF key fraction (0x00)
3. Configure operators (4 per channel):
   - `0x40 + slot`: DT1/MUL (0x01 = fundamental frequency)
   - `0x60 + slot`: TL total level (0x00 for carrier, 0x7F for silent operators)
   - Envelope: AR=31, D1R=5, D2R=5, D1L=15, RR=7
4. Key ON: `0x08` with operator mask (0x78 = all 4 operators) + channel

### Critical Timing Constants
- `CYCLES_PER_SAMPLE`: 64 (standard emulation ratio)
- `REGISTER_WRITE_DELAY_CYCLES`: 12 (essential delay between writes)

## Audio Output Patterns

### Buffer Validation (Phase1)
- Generate `TOTAL_SAMPLES * 2` (stereo) into `int32_t` buffer
- Validate with `max_abs >= 100` and `non_zero_count > 0`
- Success criteria: "Buffer is NON-SILENT!"

### WAV Output (Phase2)
- Convert int32 samples to int16 for WAV format: `sample = (int16_t)(output[i] >> 16)`
- WAV structure: Header + FMT chunk + DATA chunk
- Default filename: `output.wav`, accepts command line argument

### Real-time Audio (Phase3)
- Uses single-header miniaudio.h (Public Domain/MIT-0)
- **Critical pattern**: Data callback-driven with `AudioContext` state
- Sample counting: Track `samples_played` vs `total_samples` for duration control
- **Buffer management**: Fill frames, then silence when done

## Integration & Dependencies

### External Libraries
- **Nuked-OPM**: Already integrated in `opm.c`/`opm.h` (LGPL 2.1)
- **MiniAudio**: Single header `src/phase3/miniaudio.h` (dual license)
- **No external package management**: All dependencies are vendored

### Cross-Platform Considerations
- Windows: `.exe` extension, no special audio libs needed
- Linux: Requires `pthread` and `dl` for miniaudio
- Use `zig cc` for seamless cross-compilation

## Testing & Validation

### Success Indicators
- Phase1: "✅ SUCCESS: Buffer is NON-SILENT!"
- Phase2: "✅ Audio buffer contains valid audio data" + WAV file created
- Phase3: Audible 440Hz tone for 3 seconds

### Common Issues
- **Silent output**: Missing register write delays
- **Compilation errors**: Missing `-lm` flag or wrong dependencies for phase3

## File Organization

### Key Files by Purpose
- `build.py`: Single build script for all phases and platforms
- `opm.{c,h}`: Core YM2151 emulation (never modify)
- `src/phase*/`: Progressive feature implementations
- `issue-notes/`: Development notes and problem resolution
- `generated-docs/`: Auto-generated project documentation

### Code Patterns
- Use `write_register_with_delay()` helper for all YM2151 register access
- Always allocate stereo buffers (`samples * 2`)
- Follow existing error handling patterns (fprintf + return 1)
- Use consistent sample rate: 44100Hz across all phases

## userからの指示
- PRコメントはuserレビュー負荷を下げるため日本語で行うこと。
   - それ以外は、ハルシネーション確率を下げるために英語で行うこと。
