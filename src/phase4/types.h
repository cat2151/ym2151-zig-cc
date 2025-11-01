#ifndef CONSTANTS_H
#define CONSTANTS_H

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include "../../opm.h"

// Sample rate and clock settings
#define OPM_CLOCK 3579545
#define CYCLES_PER_SAMPLE 64
#define INTERNAL_SAMPLE_RATE (OPM_CLOCK / CYCLES_PER_SAMPLE)  // ~55930 Hz
#define OUTPUT_SAMPLE_RATE 48000  // Output device sample rate

#define BPM 120
#define REGISTER_WRITE_DELAY_CYCLES 128
#define DELAY_SAMPLES ((REGISTER_WRITE_DELAY_CYCLES) / (CYCLES_PER_SAMPLE))  // 2 samples

// Internal buffer size for resampler
#define INTERNAL_BUFFER_SIZE 4096

// Register write event structure
typedef struct {
    uint32_t sample_time;  // Time in samples from start (not delta)
    uint8_t address;
    uint8_t data;
} RegisterEvent;

// Dynamic array for register events
typedef struct {
    RegisterEvent* events;
    size_t count;
    size_t capacity;
} RegisterEventList;

// User data structure for MiniAudio callback
typedef struct {
    opm_t chip;
    uint32_t samples_played;
    uint32_t total_samples;
    int is_playing;
    ma_resampler resampler;
    int16_t internal_buffer[INTERNAL_BUFFER_SIZE * 2];  // Stereo buffer
    RegisterEventList* events;
    size_t next_event_index;
    int32_t* wav_buffer;  // Buffer for WAV output
    size_t wav_buffer_pos;
} AudioContext;

// WAV file structures
typedef struct {
    char riff[4];
    uint32_t file_size;
    char wave[4];
} WAVHeader;

typedef struct {
    char fmt[4];
    uint32_t chunk_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
} FMTChunk;

typedef struct {
    char data[4];
    uint32_t data_size;
} DATAChunk;

#endif // CONSTANTS_H
