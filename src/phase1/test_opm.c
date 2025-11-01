/* Test program for Nuked-OPM
 * Renders 440Hz tone for 3 seconds and checks if buffer is non-silent
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "../../opm.h"

// Sample rate and clock settings
#define OPM_CLOCK 3579545
// #define OPM_CLOCK 4000000 // issue #22
#define CYCLES_PER_SAMPLE 64
#define SAMPLE_RATE (OPM_CLOCK / CYCLES_PER_SAMPLE)

#define DURATION_SECONDS 3
#define TOTAL_SAMPLES (SAMPLE_RATE * DURATION_SECONDS)
#define REGISTER_WRITE_DELAY_CYCLES 128

// Helper function to write register with delay
void write_register_with_delay(opm_t *chip, uint8_t addr, uint8_t data, int32_t *dummy_output)
{
    // Write address
    OPM_Write(chip, 0, addr);

    for (uint32_t i = 0; i < REGISTER_WRITE_DELAY_CYCLES; i++)
    {
        OPM_Clock(chip, dummy_output, NULL, NULL, NULL);
    }

    // Write data
    OPM_Write(chip, 1, data);

    for (uint32_t i = 0; i < REGISTER_WRITE_DELAY_CYCLES; i++)
    {
        OPM_Clock(chip, dummy_output, NULL, NULL, NULL);
    }

    // Debug output for key register writes
    if (addr == 0x08)
    {
        printf("    Wrote KEY ON register: addr=0x%02X data=0x%02X (IC=%d)\n", addr, data, chip->ic);
    }
}

// Configure OPM for 440Hz tone on channel 0
void configure_440hz_tone(opm_t *chip)
{
    int32_t dummy_output[2] = {0, 0};

    printf("  Writing registers...\n");

    int channel = 0;

    // Reset all channels (key off)
    for (int ch = 0; ch < 8; ch++)
    {
        write_register_with_delay(chip, 0x08, ch, dummy_output);
    }

    // RL_FB_CONNECT: RL=11 (both L/R), FB=0, CON=7 (all carriers direct to output)
    write_register_with_delay(chip, 0x20 + channel, 0xC7, dummy_output);

    // Set frequency (440 Hz = A4)
    // KC (Key Code) for A4 (440Hz): 0x4A ※OPM_CLOCK 3579545 の場合のみ。違うときはKC/KFともに変更しないと違うピッチになる
    write_register_with_delay(chip, 0x28 + channel, 0x4A, dummy_output);

    // KF (Key Fraction)
    write_register_with_delay(chip, 0x30 + channel, 0x00, dummy_output);

    // PMS/AMS (Phase/Amplitude Modulation Sensitivity)
    write_register_with_delay(chip, 0x38 + channel, 0x00, dummy_output);

    // Configure all 4 operators for channel 0
    for (int op = 0; op < 4; op++)
    {
        int slot = channel + (op * 8);

        // DT1/MUL: DT1=0, MUL=1 (fundamental frequency)
        write_register_with_delay(chip, 0x40 + slot, 0x01, dummy_output);

        // TL (Total Level) - 0x00 for op0 (max volume), 0x7F for others
        if (op == 0)
        {
            write_register_with_delay(chip, 0x60 + slot, 0x00, dummy_output); // Max volume for carrier
        }
        else
        {
            write_register_with_delay(chip, 0x60 + slot, 0x7F, dummy_output); // Silent for others
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

int main()
{
    printf("Nuked-OPM 440Hz Test Program\n");
    printf("=============================\n\n");

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
    if (!buffer)
    {
        fprintf(stderr, "Failed to allocate audio buffer\n");
        return 1;
    }

    // Render audio
    printf("Rendering %d seconds of audio...\n", DURATION_SECONDS);
    int first_nonzero = -1;
    for (int i = 0; i < TOTAL_SAMPLES; i++)
    {
        int32_t output[2] = {0, 0};

        // Clock the chip multiple times per sample
        for (int j = 0; j < CYCLES_PER_SAMPLE; j++)
        {
            OPM_Clock(&chip, output, NULL, NULL, NULL);
        }

        buffer[i * 2] = output[0];
        buffer[i * 2 + 1] = output[1];

        // Debug: report first non-zero sample
        if (first_nonzero < 0 && (output[0] != 0 || output[1] != 0))
        {
            first_nonzero = i;
            printf("  First non-zero sample at index %d: L=%d R=%d\n", i, output[0], output[1]);
        }
    }

    if (first_nonzero < 0)
    {
        printf("  WARNING: No non-zero samples detected during rendering!\n");
    }

    // Check if buffer contains non-silent audio
    printf("\nAnalyzing rendered audio buffer...\n");

    int64_t sum_abs = 0;
    int32_t max_abs = 0;
    int non_zero_count = 0;

    for (int i = 0; i < TOTAL_SAMPLES * 2; i++)
    {
        // Handle potential overflow for INT32_MIN
        int64_t abs_val = (int64_t)(buffer[i] < 0 ? -(int64_t)buffer[i] : buffer[i]);
        sum_abs += abs_val;
        if (abs_val > max_abs)
        {
            max_abs = (int32_t)abs_val;
        }
        if (buffer[i] != 0)
        {
            non_zero_count++;
        }
    }

    double avg_abs = (double)sum_abs / (TOTAL_SAMPLES * 2);

    printf("  Total samples: %d\n", TOTAL_SAMPLES * 2);
    printf("  Non-zero samples: %d (%.2f%%)\n", non_zero_count,
           (double)non_zero_count * 100.0 / (TOTAL_SAMPLES * 2));
    printf("  Max absolute value: %d\n", max_abs);
    printf("  Average absolute value: %.2f\n", avg_abs);

    // Determine if audio is silent
    int is_silent = (non_zero_count == 0) || (max_abs < 100);

    printf("\n");
    if (is_silent)
    {
        printf("❌ FAILED: Buffer is SILENT!\n");
        printf("The audio buffer does not contain meaningful audio data.\n");
        free(buffer);
        return 1;
    }
    else
    {
        printf("✅ SUCCESS: Buffer is NON-SILENT!\n");
        printf("The audio buffer contains audio data.\n");
    }

    free(buffer);
    return 0;
}
