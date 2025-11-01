/* WAV output program for Nuked-OPM
 * Renders 440Hz tone for 3 seconds and outputs to WAV file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "../../opm.h"

// Sample rate and clock settings
#define SAMPLE_RATE 44100
#define OPM_CLOCK 3579545  // OPM clock frequency (3.579545 MHz)
#define CYCLES_PER_SAMPLE 64  // YM2151 chip cycles per internal sample (fixed)
#define DURATION_SECONDS 3
#define TOTAL_SAMPLES (SAMPLE_RATE * DURATION_SECONDS)
#define NUM_CHANNELS 2  // Stereo

// Delay calculations
#define MS_TO_CYCLES(ms) ((OPM_CLOCK * (ms)) / 1000)
#define INIT_DELAY_MS 100
#define REGISTER_WRITE_DELAY_MS 10

// WAV file header structures
typedef struct {
    char riff[4];           // "RIFF"
    uint32_t file_size;     // File size - 8
    char wave[4];           // "WAVE"
} WAVHeader;

typedef struct {
    char fmt[4];            // "fmt "
    uint32_t chunk_size;    // Size of fmt chunk (16 for PCM)
    uint16_t audio_format;  // Audio format (1 = PCM)
    uint16_t num_channels;  // Number of channels
    uint32_t sample_rate;   // Sample rate
    uint32_t byte_rate;     // Byte rate
    uint16_t block_align;   // Block align
    uint16_t bits_per_sample; // Bits per sample
} FMTChunk;

typedef struct {
    char data[4];           // "data"
    uint32_t data_size;     // Size of data
} DATAChunk;

// Helper function to write register with delay
void write_register_with_delay(opm_t *chip, uint8_t addr, uint8_t data, int32_t *dummy_output) {
    // Write address
    OPM_Write(chip, 0, addr);
    
    // Delay 10ms worth of cycles
    uint32_t delay_cycles = MS_TO_CYCLES(REGISTER_WRITE_DELAY_MS);
    for (uint32_t i = 0; i < delay_cycles; i++) {
        OPM_Clock(chip, dummy_output, NULL, NULL, NULL);
    }
    
    // Write data
    OPM_Write(chip, 1, data);
    
    // Delay 10ms worth of cycles
    for (uint32_t i = 0; i < delay_cycles; i++) {
        OPM_Clock(chip, dummy_output, NULL, NULL, NULL);
    }
}

// Helper function to calculate KC (Key Code) from MIDI note number
// KC is clock-independent - it's a musical note representation
uint8_t calculate_kc(int midi_note) {
    // Calculate octave and note within octave
    // YM2151 octave range is 0-7, note range is 0-11 for chromatic scale
    int octave = midi_note / 12;
    int note = midi_note % 12;
    
    // Clamp octave to valid range 0-7
    if (octave < 0) octave = 0;
    if (octave > 7) octave = 7;
    
    // KC format: upper 3 bits = octave, lower 4 bits = note
    // Note is mapped as: C=0, C#=1, D=2, ..., B=11
    uint8_t kc = ((octave & 0x07) << 4) | (note & 0x0F);
    
    return kc;
}

// Helper function to calculate KF (Key Fraction) for fine tuning
// KF provides fine tuning between semitones (0-63)
// For standard MIDI notes without pitch bend, kf_fraction should be 0.0
uint8_t calculate_kf(double kf_fraction) {
    // KF is 6 bits (0-63), representing the fraction of a semitone
    // kf_fraction should be in range [0.0, 1.0)
    uint8_t kf = (uint8_t)(kf_fraction * 64.0);
    
    // Clamp to 6-bit range
    if (kf > 63) kf = 63;
    
    return kf;
}

// Configure OPM for 440Hz tone on channel 0
void configure_440hz_tone(opm_t *chip) {
    int32_t dummy_output[2] = {0, 0};
    
    printf("  Writing registers...\n");
    
    int channel = 0;
    
    // Reset all channels (key off)
    for (int ch = 0; ch < 8; ch++) {
        write_register_with_delay(chip, 0x08, ch, dummy_output);
    }
    
    // RL_FB_CONNECT: RL=11 (both L/R), FB=0, CON=7 (all carriers direct to output)
    write_register_with_delay(chip, 0x20 + channel, 0xC7, dummy_output);
    
    // Set frequency (440 Hz = A4 = MIDI note 69)
    // Calculate KC (Key Code) and KF (Key Fraction) dynamically from MIDI note number
    int midi_note = 69;  // A4 (440 Hz)
    uint8_t kc = calculate_kc(midi_note);
    uint8_t kf = calculate_kf(0.0);  // No fine tuning for standard MIDI note
    
    printf("  Calculated KC=0x%02X, KF=0x%02X for MIDI note %d (440Hz)\n", 
           kc, kf, midi_note);
    
    write_register_with_delay(chip, 0x28 + channel, kc, dummy_output);
    write_register_with_delay(chip, 0x30 + channel, kf, dummy_output);
    
    // PMS/AMS (Phase/Amplitude Modulation Sensitivity)
    write_register_with_delay(chip, 0x38 + channel, 0x00, dummy_output);
    
    // Configure all 4 operators for channel 0
    for (int op = 0; op < 4; op++) {
        int slot = channel + (op * 8);
        
        // DT1/MUL: DT1=0, MUL=1 (fundamental frequency)
        write_register_with_delay(chip, 0x40 + slot, 0x01, dummy_output);
        
        // TL (Total Level) - 0x00 for op0 (max volume), 0x7F for others
        if (op == 0) {
            write_register_with_delay(chip, 0x60 + slot, 0x00, dummy_output);  // Max volume for carrier
        } else {
            write_register_with_delay(chip, 0x60 + slot, 0x7F, dummy_output);  // Silent for others
        }
        
        // KS/AR: KS=0, AR=31 (maximum attack rate)
        write_register_with_delay(chip, 0x80 + slot, 0x1F, dummy_output);
        
        // AMS/D1R: AMS=0, D1R=5
        write_register_with_delay(chip, 0xA0 + slot, 0x05, dummy_output);
        
        // DT2/D2R: DT2=0, D2R=5
        write_register_with_delay(chip, 0xC0 + slot, 0x05, dummy_output);
        
        // D1L/RR: D1L=15, RR=7
        write_register_with_delay(chip, 0xE0 + slot, 0xF7, dummy_output);
    }
    
    printf("  Key ON channel 0, all operators...\n");
    // Key ON: trigger channel 0, all 4 operators (bits 6,5,4,3 = 0x78)
    write_register_with_delay(chip, 0x08, 0x78 | channel, dummy_output);
    
    printf("  Configuration complete.\n");
}

// Write WAV file
int write_wav_file(const char *filename, int32_t *buffer, uint32_t num_samples) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s' for writing\n", filename);
        return 0;
    }
    
    printf("Writing WAV file: %s\n", filename);
    
    uint32_t data_size = num_samples * NUM_CHANNELS * sizeof(int16_t);
    uint32_t file_size = sizeof(WAVHeader) + sizeof(FMTChunk) + sizeof(DATAChunk) + data_size;
    
    // Write WAV header
    WAVHeader wav_header;
    memcpy(wav_header.riff, "RIFF", 4);
    wav_header.file_size = file_size - 8;
    memcpy(wav_header.wave, "WAVE", 4);
    fwrite(&wav_header, sizeof(WAVHeader), 1, file);
    
    // Write FMT chunk
    FMTChunk fmt_chunk;
    memcpy(fmt_chunk.fmt, "fmt ", 4);
    fmt_chunk.chunk_size = 16;
    fmt_chunk.audio_format = 1;  // PCM
    fmt_chunk.num_channels = NUM_CHANNELS;
    fmt_chunk.sample_rate = SAMPLE_RATE;
    fmt_chunk.bits_per_sample = 16;
    fmt_chunk.block_align = NUM_CHANNELS * fmt_chunk.bits_per_sample / 8;
    fmt_chunk.byte_rate = fmt_chunk.sample_rate * fmt_chunk.block_align;
    fwrite(&fmt_chunk, sizeof(FMTChunk), 1, file);
    
    // Write DATA chunk header
    DATAChunk data_chunk;
    memcpy(data_chunk.data, "data", 4);
    data_chunk.data_size = data_size;
    fwrite(&data_chunk, sizeof(DATAChunk), 1, file);
    
    // Convert 32-bit samples to 16-bit and write
    printf("  Converting and writing audio data...\n");
    for (uint32_t i = 0; i < num_samples * NUM_CHANNELS; i++) {
        // Convert 32-bit to 16-bit by dividing by 2 (simple conversion)
        int16_t sample_16 = (int16_t)(buffer[i] / 2);
        fwrite(&sample_16, sizeof(int16_t), 1, file);
    }
    
    fclose(file);
    
    printf("  WAV file written successfully!\n");
    printf("  File size: %u bytes\n", file_size);
    printf("  Duration: %d seconds\n", DURATION_SECONDS);
    printf("  Sample rate: %d Hz\n", SAMPLE_RATE);
    printf("  Channels: %d (stereo)\n", NUM_CHANNELS);
    printf("  Bits per sample: 16\n");
    
    return 1;
}

int main(int argc, char *argv[]) {
    printf("Nuked-OPM 440Hz WAV Output Program\n");
    printf("===================================\n\n");
    
    // Get output filename from command line or use default
    const char *output_file = "output.wav";
    if (argc > 1) {
        output_file = argv[1];
    }
    
    // Allocate chip structure
    opm_t chip;
    memset(&chip, 0, sizeof(opm_t));
    
    // Reset chip
    OPM_Reset(&chip);
    
    // Configure for 440Hz tone
    printf("Configuring OPM for 440Hz tone...\n");
    configure_440hz_tone(&chip);
    
    // Allocate buffer for rendered audio
    int32_t *buffer = (int32_t *)malloc(TOTAL_SAMPLES * 2 * sizeof(int32_t));
    if (!buffer) {
        fprintf(stderr, "Failed to allocate audio buffer\n");
        return 1;
    }
    
    // Render audio
    printf("\nRendering %d seconds of audio...\n", DURATION_SECONDS);
    int first_nonzero = -1;
    for (int i = 0; i < TOTAL_SAMPLES; i++) {
        int32_t output[2] = {0, 0};
        
        // Clock the chip multiple times per sample
        for (int j = 0; j < CYCLES_PER_SAMPLE; j++) {
            OPM_Clock(&chip, output, NULL, NULL, NULL);
        }
        
        buffer[i * 2] = output[0];
        buffer[i * 2 + 1] = output[1];
        
        // Debug: report first non-zero sample
        if (first_nonzero < 0 && (output[0] != 0 || output[1] != 0)) {
            first_nonzero = i;
            printf("  First non-zero sample at index %d: L=%d R=%d\n", i, output[0], output[1]);
        }
    }
    
    if (first_nonzero < 0) {
        printf("  WARNING: No non-zero samples detected during rendering!\n");
    }
    
    // Analyze buffer
    printf("\nAnalyzing rendered audio buffer...\n");
    
    int64_t sum_abs = 0;
    int32_t max_abs = 0;
    int non_zero_count = 0;
    
    for (int i = 0; i < TOTAL_SAMPLES * 2; i++) {
        int64_t abs_val = (int64_t)(buffer[i] < 0 ? -(int64_t)buffer[i] : buffer[i]);
        sum_abs += abs_val;
        if (abs_val > max_abs) {
            max_abs = (int32_t)abs_val;
        }
        if (buffer[i] != 0) {
            non_zero_count++;
        }
    }
    
    double avg_abs = (double)sum_abs / (TOTAL_SAMPLES * 2);
    
    printf("  Total samples: %d\n", TOTAL_SAMPLES * 2);
    printf("  Non-zero samples: %d (%.2f%%)\n", non_zero_count, 
           (double)non_zero_count * 100.0 / (TOTAL_SAMPLES * 2));
    printf("  Max absolute value: %d\n", max_abs);
    printf("  Average absolute value: %.2f\n", avg_abs);
    
    // Check if audio is silent
    int is_silent = (non_zero_count == 0) || (max_abs < 100);
    
    if (is_silent) {
        printf("\n❌ ERROR: Buffer is SILENT!\n");
        printf("Cannot write WAV file with silent audio.\n");
        free(buffer);
        return 1;
    }
    
    printf("\n✅ Audio buffer contains valid audio data.\n\n");
    
    // Write WAV file
    if (!write_wav_file(output_file, buffer, TOTAL_SAMPLES)) {
        free(buffer);
        return 1;
    }
    
    printf("\n✅ SUCCESS!\n");
    printf("WAV file '%s' created successfully.\n", output_file);
    
    free(buffer);
    return 0;
}
