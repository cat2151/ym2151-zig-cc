/* Phase4: BPM120 music sequence player with 2-pass register data generation
 * Plays MIDI notes 60, 64, 67, 71 (C4, E4, G4, B4) as quarter notes
 * Features:
 * - 2-pass processing: musical data -> register writes with delays
 * - JSON output for testing
 * - Real-time playback with WAV file output
 */

#include "types.h"
#include "events.h"
#include "core.h"
#include "wav_writer.h"

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
