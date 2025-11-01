/* Real-time audio playback program using Nuked-OPM and MiniAudio
 * Plays 440Hz tone for 3 seconds in real-time
 */

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include "../../opm.h"

// Sample rate and clock settings
#define OPM_CLOCK 3579545
// #define OPM_CLOCK 4000000 // issue #22
#define CYCLES_PER_SAMPLE 64
#define SAMPLE_RATE (OPM_CLOCK / CYCLES_PER_SAMPLE)

#define DURATION_SECONDS 3
#define REGISTER_WRITE_DELAY_CYCLES 128

// User data structure for MiniAudio callback
typedef struct {
    opm_t chip;
    uint32_t samples_played;
    uint32_t total_samples;
    int is_playing;
} AudioContext;

// Helper function to write register with delay
void write_register_with_delay(opm_t *chip, uint8_t addr, uint8_t data, int32_t *dummy_output) {
    // Write address
    OPM_Write(chip, 0, addr);
    
    for (uint32_t i = 0; i < REGISTER_WRITE_DELAY_CYCLES; i++) {
        OPM_Clock(chip, dummy_output, NULL, NULL, NULL);
    }
    
    // Write data
    OPM_Write(chip, 1, data);
    
    for (uint32_t i = 0; i < REGISTER_WRITE_DELAY_CYCLES; i++) {
        OPM_Clock(chip, dummy_output, NULL, NULL, NULL);
    }
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
    
    // Set frequency (440 Hz = A4)
    // KC (Key Code) for A4 (440Hz): 0x4A
    write_register_with_delay(chip, 0x28 + channel, 0x4A, dummy_output);
    
    // KF (Key Fraction)
    write_register_with_delay(chip, 0x30 + channel, 0x00, dummy_output);
    
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

// MiniAudio data callback - called by audio thread to fill the buffer
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    AudioContext* pContext = (AudioContext*)pDevice->pUserData;
    int16_t* pOutputS16 = (int16_t*)pOutput;
    
    (void)pInput; // Unused parameter
    
    if (!pContext->is_playing) {
        // Fill with silence if not playing
        memset(pOutput, 0, frameCount * 2 * sizeof(int16_t));
        return;
    }
    
    for (ma_uint32 i = 0; i < frameCount; i++) {
        // Check if we've played enough samples
        if (pContext->samples_played >= pContext->total_samples) {
            // Fill remaining with silence
            for (ma_uint32 j = i; j < frameCount; j++) {
                pOutputS16[j * 2] = 0;
                pOutputS16[j * 2 + 1] = 0;
            }
            pContext->is_playing = 0;
            break;
        }
        
        // Generate one stereo sample from OPM chip
        int32_t output[2] = {0, 0};
        
        // Clock the chip multiple times per sample
        for (int j = 0; j < CYCLES_PER_SAMPLE; j++) {
            OPM_Clock(&pContext->chip, output, NULL, NULL, NULL);
        }
        
        // Convert 32-bit samples to 16-bit by dividing by 2
        pOutputS16[i * 2] = (int16_t)(output[0] / 2);
        pOutputS16[i * 2 + 1] = (int16_t)(output[1] / 2);
        
        pContext->samples_played++;
    }
}

int main() {
    printf("Nuked-OPM Real-Time Audio Program (MiniAudio)\n");
    printf("==============================================\n\n");
    
    // Initialize audio context
    AudioContext context;
    memset(&context, 0, sizeof(AudioContext));
    
    // Reset OPM chip
    OPM_Reset(&context.chip);
    
    // Configure for 440Hz tone
    printf("Configuring OPM for 440Hz tone...\n");
    configure_440hz_tone(&context.chip);
    
    // Set playback parameters
    context.samples_played = 0;
    context.total_samples = SAMPLE_RATE * DURATION_SECONDS;
    context.is_playing = 1;
    
    printf("\nInitializing MiniAudio device...\n");
    
    // Configure MiniAudio device
    ma_device_config deviceConfig;
    ma_device device;
    
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = ma_format_s16;
    deviceConfig.playback.channels = 2;
    deviceConfig.sampleRate        = SAMPLE_RATE;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = &context;
    
    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        fprintf(stderr, "❌ Failed to initialize MiniAudio device\n");
        return 1;
    }
    
    printf("✅ MiniAudio device initialized successfully\n");
    printf("   Sample rate: %d Hz\n", device.sampleRate);
    printf("   Channels: %d (stereo)\n", device.playback.channels);
    printf("   Format: 16-bit signed integer\n");
    
    printf("\nStarting playback...\n");
    printf("Playing 440Hz tone for %d seconds...\n", DURATION_SECONDS);
    
    if (ma_device_start(&device) != MA_SUCCESS) {
        fprintf(stderr, "❌ Failed to start MiniAudio device\n");
        ma_device_uninit(&device);
        return 1;
    }
    
    // Wait until playback is done
    while (context.is_playing) {
        ma_sleep(100); // Sleep for 100ms to avoid busy waiting
    }
    
    // Small additional delay to ensure buffer is fully drained
    ma_sleep(200);
    
    printf("\n✅ Playback completed!\n");
    printf("   Total samples played: %u\n", context.samples_played);
    
    // Cleanup
    ma_device_uninit(&device);
    
    printf("\n✅ SUCCESS!\n");
    printf("Real-time audio playback completed successfully.\n");
    
    return 0;
}
