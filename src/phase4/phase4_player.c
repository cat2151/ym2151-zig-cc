/* Phase4: BPM120 music sequence player with 2-pass register data generation
 * Plays MIDI notes 60, 64, 67, 71 (C4, E4, G4, B4) as quarter notes
 * Features:
 * - 2-pass processing: musical data -> register writes with delays
 * - JSON output for testing
 * - Real-time playback with WAV file output
 */

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

// Calculate samples for a duration at internal sample rate
uint32_t duration_to_samples(double duration_seconds) {
    return (uint32_t)(duration_seconds * INTERNAL_SAMPLE_RATE);
}

// Initialize register event list
RegisterEventList* create_event_list() {
    RegisterEventList* list = (RegisterEventList*)malloc(sizeof(RegisterEventList));
    list->capacity = 256;
    list->count = 0;
    list->events = (RegisterEvent*)malloc(sizeof(RegisterEvent) * list->capacity);
    return list;
}

// Add event to list
void add_event(RegisterEventList* list, uint32_t sample_time, uint8_t address, uint8_t data) {
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        RegisterEvent* new_events = (RegisterEvent*)realloc(list->events, sizeof(RegisterEvent) * list->capacity);
        if (!new_events) {
            fprintf(stderr, "❌ Failed to reallocate memory for events\n");
            exit(1);
        }
        list->events = new_events;
    }
    list->events[list->count].sample_time = sample_time;
    list->events[list->count].address = address;
    list->events[list->count].data = data;
    list->count++;
}

// Free event list
void free_event_list(RegisterEventList* list) {
    free(list->events);
    free(list);
}

// MIDI note to YM2151 KC/KF conversion
void midi_to_kc_kf(uint8_t midi_note, uint8_t* kc, uint8_t* kf) {
    // Note table mapping MIDI note in octave to YM2151 NOTE field
    // Based on empirical testing with 440Hz (A4) = KC 0x4A
    const uint8_t note_table[12] = {
        0,   // C
        1,   // C#
        2,   // D
        4,   // D#
        5,   // E
        6,   // F (corrected to match YM2151 spec)
        6,   // F#
        8,   // G
        9,   // G#
        10,  // A
        12,  // A#
        13   // B
    };
    
    uint8_t midi_octave = (midi_note / 12) - 1;
    uint8_t note_in_octave = midi_note % 12;
    uint8_t ym_note = note_table[note_in_octave];
    
    *kc = (midi_octave << 4) | ym_note;
    *kf = 0;  // No fine tuning for now (could be used to distinguish F from F#)
}

// Pass 1: Generate musical events (no delays)
RegisterEventList* generate_pass1_events() {
    RegisterEventList* list = create_event_list();
    
    // Calculate timing
    double quarter_note_duration = 60.0 / BPM;  // 0.5 seconds for BPM 120
    uint32_t quarter_note_samples = duration_to_samples(quarter_note_duration);
    
    printf("Pass 1: Generating musical events\n");
    printf("  Quarter note duration: %.4f seconds (%u samples)\n", 
           quarter_note_duration, quarter_note_samples);
    
    // Initialize: Reset all channels
    for (int ch = 0; ch < 8; ch++) {
        add_event(list, 0, 0x08, ch);
    }
    
    // Configure channel 0 (only once, before playing any notes)
    int channel = 0;
    
    // RL_FB_CONNECT
    add_event(list, 0, 0x20 + channel, 0xC7);
    
    // PMS/AMS
    add_event(list, 0, 0x38 + channel, 0x00);
    
    // Configure operators (once for the channel)
    for (int op = 0; op < 4; op++) {
        int slot = channel + (op * 8);
        
        // DT1/MUL
        add_event(list, 0, 0x40 + slot, 0x01);
        
        // TL (Total Level)
        if (op == 0) {
            add_event(list, 0, 0x60 + slot, 0x00);  // Max volume for carrier
        } else {
            add_event(list, 0, 0x60 + slot, 0x7F);  // Silent for others
        }
        
        // KS/AR
        add_event(list, 0, 0x80 + slot, 0x1F);
        
        // AMS/D1R
        add_event(list, 0, 0xA0 + slot, 0x05);
        
        // DT2/D2R
        add_event(list, 0, 0xC0 + slot, 0x05);
        
        // D1L/RR
        add_event(list, 0, 0xE0 + slot, 0xF7);
    }
    
    // Play sequence: MIDI notes 60, 64, 67, 71 (C4, E4, G4, B4)
    uint8_t notes[] = {60, 64, 67, 71};
    const char* note_names[] = {"C4", "E4", "G4", "B4"};
    
    for (int i = 0; i < 4; i++) {
        uint32_t note_start_time = i * quarter_note_samples;
        uint8_t kc, kf;
        midi_to_kc_kf(notes[i], &kc, &kf);
        
        printf("  Note %d (%s, MIDI %d): start=%u samples, KC=0x%02X, KF=0x%02X\n",
               i, note_names[i], notes[i], note_start_time, kc, kf);
        
        // Set KC (Key Code)
        add_event(list, note_start_time, 0x28 + channel, kc);
        
        // Set KF (Key Fraction)
        add_event(list, note_start_time, 0x30 + channel, kf);
        
        // Key ON
        add_event(list, note_start_time, 0x08, 0x78 | channel);
        
        // Key OFF (at end of quarter note)
        uint32_t note_end_time = (i + 1) * quarter_note_samples;
        add_event(list, note_end_time, 0x08, channel);
    }
    
    printf("  Pass 1 complete: %zu events\n\n", list->count);
    return list;
}

// Pass 2: Add register write delays
RegisterEventList* generate_pass2_events(RegisterEventList* pass1) {
    RegisterEventList* list = create_event_list();
    
    printf("Pass 2: Adding register write delays\n");
    printf("  Delay per register write: %d samples\n", DELAY_SAMPLES);
    
    uint32_t accumulated_delay = 0;
    uint32_t last_time = 0;
    
    for (size_t i = 0; i < pass1->count; i++) {
        RegisterEvent* event = &pass1->events[i];
        
        // If this event is at a different musical time, reset accumulated delay
        if (event->sample_time != last_time) {
            accumulated_delay = 0;
            last_time = event->sample_time;
        }
        
        // Add event with accumulated delay
        uint32_t actual_time = event->sample_time + accumulated_delay;
        add_event(list, actual_time, event->address, event->data);
        
        // Accumulate delay for next register write
        accumulated_delay += DELAY_SAMPLES;
    }
    
    printf("  Pass 2 complete: %zu events\n\n", list->count);
    return list;
}

// Calculate total playback duration from pass2 events
double calculate_playback_duration(RegisterEventList* pass2) {
    if (pass2->count == 0) return 1.0;
    
    // Find last event time
    uint32_t last_event_time = 0;
    for (size_t i = 0; i < pass2->count; i++) {
        if (pass2->events[i].sample_time > last_event_time) {
            last_event_time = pass2->events[i].sample_time;
        }
    }
    
    // Add 1 second after last event
    uint32_t total_samples = last_event_time + INTERNAL_SAMPLE_RATE;
    double duration = (double)total_samples / INTERNAL_SAMPLE_RATE;
    
    printf("Playback duration calculation:\n");
    printf("  Last event at: %u samples (%.3f seconds)\n", 
           last_event_time, (double)last_event_time / INTERNAL_SAMPLE_RATE);
    printf("  Total duration: %.3f seconds (%u samples)\n\n", duration, total_samples);
    
    return duration;
}

// Save events to JSON file
void save_events_json(const char* filename, RegisterEventList* events) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "❌ Failed to open %s for writing\n", filename);
        return;
    }
    
    fprintf(fp, "{\n");
    fprintf(fp, "  \"event_count\": %zu,\n", events->count);
    fprintf(fp, "  \"events\": [\n");
    
    for (size_t i = 0; i < events->count; i++) {
        RegisterEvent* e = &events->events[i];
        fprintf(fp, "    {\"time\": %u, \"addr\": \"0x%02X\", \"data\": \"0x%02X\"}",
                e->sample_time, e->address, e->data);
        if (i < events->count - 1) {
            fprintf(fp, ",");
        }
        fprintf(fp, "\n");
    }
    
    fprintf(fp, "  ]\n");
    fprintf(fp, "}\n");
    
    fclose(fp);
    printf("✅ Saved events to %s\n", filename);
}

// Process register events up to current sample time
void process_events_until(AudioContext* ctx, uint32_t current_sample) {
    int32_t dummy_output[2] = {0, 0};
    
    // Pass2 events specify WHEN to issue writes
    // Each write still needs the proper protocol with clock cycles
    // Note: These clock cycles are part of the write protocol, not extra delays
    
    while (ctx->next_event_index < ctx->events->count) {
        RegisterEvent* event = &ctx->events->events[ctx->next_event_index];
        
        if (event->sample_time > current_sample) {
            break;  // Haven't reached this event yet
        }
        
        // Execute the register write protocol with required delays
        OPM_Write(&ctx->chip, 0, event->address);
        for (int i = 0; i < REGISTER_WRITE_DELAY_CYCLES; i++) {
            OPM_Clock(&ctx->chip, dummy_output, NULL, NULL, NULL);
        }
        
        OPM_Write(&ctx->chip, 1, event->data);
        for (int i = 0; i < REGISTER_WRITE_DELAY_CYCLES; i++) {
            OPM_Clock(&ctx->chip, dummy_output, NULL, NULL, NULL);
        }
        
        ctx->next_event_index++;
    }
}

// MiniAudio data callback
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    AudioContext* pContext = (AudioContext*)pDevice->pUserData;
    int16_t* pOutputS16 = (int16_t*)pOutput;
    
    (void)pInput;
    
    if (!pContext->is_playing) {
        memset(pOutput, 0, frameCount * 2 * sizeof(int16_t));
        return;
    }
    
    // Calculate required input frames
    ma_uint64 requiredInputFrames = 0;
    ma_result result = ma_resampler_get_required_input_frame_count(&pContext->resampler, frameCount, &requiredInputFrames);
    if (result != MA_SUCCESS) {
        memset(pOutput, 0, frameCount * 2 * sizeof(int16_t));
        return;
    }
    
    if (requiredInputFrames > INTERNAL_BUFFER_SIZE) {
        requiredInputFrames = INTERNAL_BUFFER_SIZE;
    }
    
    // Generate internal samples
    ma_uint64 actualInputFrames = 0;
    for (ma_uint64 i = 0; i < requiredInputFrames; i++) {
        if (pContext->samples_played >= pContext->total_samples) {
            // Fill rest with silence
            for (ma_uint64 j = i; j < requiredInputFrames; j++) {
                pContext->internal_buffer[j * 2] = 0;
                pContext->internal_buffer[j * 2 + 1] = 0;
            }
            actualInputFrames = requiredInputFrames;
            pContext->is_playing = 0;
            break;
        }
        
        // Process any register events that should happen now
        process_events_until(pContext, pContext->samples_played);
        
        // Generate one stereo sample
        int32_t output[2] = {0, 0};
        for (int j = 0; j < CYCLES_PER_SAMPLE; j++) {
            OPM_Clock(&pContext->chip, output, NULL, NULL, NULL);
        }
        
        // Store to internal buffer (convert to 16-bit)
        pContext->internal_buffer[i * 2] = (int16_t)(output[0] / 2);
        pContext->internal_buffer[i * 2 + 1] = (int16_t)(output[1] / 2);
        
        // Also store to WAV buffer (keep as 32-bit)
        if (pContext->wav_buffer) {
            pContext->wav_buffer[pContext->wav_buffer_pos * 2] = output[0];
            pContext->wav_buffer[pContext->wav_buffer_pos * 2 + 1] = output[1];
            pContext->wav_buffer_pos++;
        }
        
        pContext->samples_played++;
    }
    
    if (actualInputFrames == 0) {
        actualInputFrames = requiredInputFrames;
    }
    
    // Resample
    ma_uint64 inputFramesProcessed = actualInputFrames;
    ma_uint64 outputFramesProcessed = frameCount;
    result = ma_resampler_process_pcm_frames(&pContext->resampler,
                                             pContext->internal_buffer, &inputFramesProcessed,
                                             pOutputS16, &outputFramesProcessed);
    
    if (result != MA_SUCCESS) {
        memset(pOutput, 0, frameCount * 2 * sizeof(int16_t));
        return;
    }
    
    // Fill remaining with silence
    if (outputFramesProcessed < frameCount) {
        for (ma_uint64 i = outputFramesProcessed; i < frameCount; i++) {
            pOutputS16[i * 2] = 0;
            pOutputS16[i * 2 + 1] = 0;
        }
    }
}

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

// Save WAV file
int save_wav_file(const char* filename, int32_t* buffer, uint32_t num_samples) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "❌ Failed to open %s for writing\n", filename);
        return 0;
    }
    
    uint32_t data_size = num_samples * 2 * sizeof(int16_t);
    
    // Write WAV header
    WAVHeader header;
    memcpy(header.riff, "RIFF", 4);
    header.file_size = data_size + 36;
    memcpy(header.wave, "WAVE", 4);
    fwrite(&header, sizeof(WAVHeader), 1, fp);
    
    // Write FMT chunk
    FMTChunk fmt;
    memcpy(fmt.fmt, "fmt ", 4);
    fmt.chunk_size = 16;
    fmt.audio_format = 1;  // PCM
    fmt.num_channels = 2;  // Stereo
    fmt.sample_rate = INTERNAL_SAMPLE_RATE;
    fmt.byte_rate = INTERNAL_SAMPLE_RATE * 2 * sizeof(int16_t);
    fmt.block_align = 2 * sizeof(int16_t);
    fmt.bits_per_sample = 16;
    fwrite(&fmt, sizeof(FMTChunk), 1, fp);
    
    // Write DATA chunk header
    DATAChunk data;
    memcpy(data.data, "data", 4);
    data.data_size = data_size;
    fwrite(&data, sizeof(DATAChunk), 1, fp);
    
    // Write audio data (convert 32-bit to 16-bit)
    for (uint32_t i = 0; i < num_samples * 2; i++) {
        // Divide by 2 to convert from 32-bit to 16-bit range
        int16_t sample = (int16_t)(buffer[i] / 2);
        fwrite(&sample, sizeof(int16_t), 1, fp);
    }
    
    fclose(fp);
    printf("✅ Saved WAV file: %s\n", filename);
    return 1;
}

int main(int argc, char** argv) {
    printf("Phase4: BPM120 Music Sequence Player\n");
    printf("=====================================\n\n");
    
    // Generate pass1 events (musical)
    RegisterEventList* pass1 = generate_pass1_events();
    save_events_json("phase4_pass1.json", pass1);
    
    // Generate pass2 events (with delays)
    RegisterEventList* pass2 = generate_pass2_events(pass1);
    save_events_json("phase4_pass2.json", pass2);
    
    // Calculate playback duration
    double duration = calculate_playback_duration(pass2);
    uint32_t total_samples = duration_to_samples(duration);
    
    // Initialize audio context
    AudioContext context;
    memset(&context, 0, sizeof(AudioContext));
    
    // Allocate WAV buffer
    context.wav_buffer = (int32_t*)malloc(total_samples * 2 * sizeof(int32_t));
    if (!context.wav_buffer) {
        fprintf(stderr, "❌ Failed to allocate WAV buffer\n");
        return 1;
    }
    context.wav_buffer_pos = 0;
    
    // Initialize OPM chip
    OPM_Reset(&context.chip);
    
    // Set playback parameters
    context.events = pass2;
    context.next_event_index = 0;
    context.samples_played = 0;
    context.total_samples = total_samples;
    context.is_playing = 1;
    
    printf("Initializing audio...\n");
    
    // Initialize resampler
    ma_resampler_config resamplerConfig = ma_resampler_config_init(
        ma_format_s16, 2, INTERNAL_SAMPLE_RATE, OUTPUT_SAMPLE_RATE,
        ma_resample_algorithm_linear
    );
    
    if (ma_resampler_init(&resamplerConfig, NULL, &context.resampler) != MA_SUCCESS) {
        fprintf(stderr, "❌ Failed to initialize resampler\n");
        free(context.wav_buffer);
        return 1;
    }
    
    // Configure MiniAudio device
    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = ma_format_s16;
    deviceConfig.playback.channels = 2;
    deviceConfig.sampleRate = OUTPUT_SAMPLE_RATE;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = &context;
    
    ma_device device;
    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        fprintf(stderr, "❌ Failed to initialize audio device\n");
        ma_resampler_uninit(&context.resampler, NULL);
        free(context.wav_buffer);
        return 1;
    }
    
    printf("✅ Audio initialized\n\n");
    
    // Start playback
    if (ma_device_start(&device) != MA_SUCCESS) {
        fprintf(stderr, "❌ Failed to start audio device\n");
        ma_device_uninit(&device);
        ma_resampler_uninit(&context.resampler, NULL);
        free(context.wav_buffer);
        return 1;
    }
    
    printf("▶  Playing sequence...\n");
    
    // Wait for playback to finish
    while (context.is_playing) {
        ma_sleep(100);
    }
    
    printf("■  Playback complete\n\n");
    
    // Stop and cleanup audio
    ma_device_uninit(&device);
    ma_resampler_uninit(&context.resampler, NULL);
    
    // Save WAV file
    const char* wav_filename = (argc > 1) ? argv[1] : "phase4_output.wav";
    save_wav_file(wav_filename, context.wav_buffer, context.wav_buffer_pos);
    
    // Cleanup
    free(context.wav_buffer);
    free_event_list(pass1);
    free_event_list(pass2);
    
    printf("\n✅ Phase4 complete!\n");
    return 0;
}
