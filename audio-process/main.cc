#include "modules/audio_processing/audio_processing_impl.h"
#include <stdio.h>

using namespace webrtc;
int main() {
    rtc::scoped_refptr<AudioProcessing> apm = AudioProcessingBuilder().Create();
    AudioProcessing::Config config;
    config.echo_canceller.enabled = true;
    config.echo_canceller.mobile_mode = false;

    config.gain_controller1.enabled = true;
    config.gain_controller1.mode =
        AudioProcessing::Config::GainController1::kAdaptiveAnalog;
    
    config.gain_controller2.enabled = true;

    config.high_pass_filter.enabled = true;

    config.voice_detection.enabled = true;

    config.transient_suppression.enabled = true;
    config.noise_suppression.enabled = true;
    config.noise_suppression.level = AudioProcessing::Config::NoiseSuppression::kVeryHigh;

    if (apm == nullptr) {
        printf("apm create failed!\n");
        return -1;
    }
    const int sample_rate = 48000;
    const int channels = 2;
    ProcessingConfig processing_config = {
    {{sample_rate, channels}, {sample_rate, channels}, {sample_rate, channels}, {sample_rate, channels}} };

    if (apm->Initialize(processing_config) != 0) {
        printf("apm Initialize failed!\n");
        return -1;
    }

    apm->ApplyConfig(config);

   
    const StreamConfig input_config(sample_rate, channels);
    const StreamConfig ouput_config(sample_rate, channels);
    FILE* input = fopen("noise.pcm", "rb");
    FILE* outfile = fopen("denoise.pcm", "wb");
    const int kSamples = 480 * channels;
    short input_samples[kSamples];
    int ret;
    //short output_samples[kSamples];
    while (!feof(input)) {
        int read = fread(input_samples, sizeof(short), kSamples, input);
        if (read == kSamples) {
            apm->ProcessReverseStream(input_samples, input_config, ouput_config, input_samples);
            apm->set_stream_delay_ms(0);
            ret = apm->ProcessStream(input_samples, input_config, ouput_config, input_samples);
            printf("ret %d\n", ret);
            fwrite(input_samples, sizeof(short), kSamples, outfile);
        } else {
            break;
        }
    }
   
    return 0;
}