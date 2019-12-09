
#include <SDL2/SDL.h>
#include <iostream>
#include <cstdint>
#include <limits>

/*
 This program is intended as a template for basic sound synthesis with SDL2
*/


// prototype for our audio callback
// see the implementation for more information
void my_audio_callback(void *userdata, Uint8 *stream, int len);

/*
** PLAYING A SOUND IS MUCH MORE COMPLICATED THAN IT SHOULD BE
*/
int main(int argc, char* argv[]){

	// Initialize SDL.
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
			return 1;

	SDL_AudioSpec desired_spec; // the specs of our piece of music
	
    unsigned int sample_counter = 0;

	// set the callback function
	desired_spec.callback = my_audio_callback;
	desired_spec.userdata = &sample_counter;
    desired_spec.samples  = 512;
    desired_spec.freq     = 44100;
    desired_spec.format   = AUDIO_S16;

    desired_spec.channels = 1;

    SDL_AudioSpec obtained_spec;

    auto device = SDL_OpenAudioDevice(nullptr, 0, &desired_spec, &obtained_spec, SDL_AUDIO_ALLOW_ANY_CHANGE);

	/* Open the audio device */
	if (device == 0){
	  fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
	  exit(-1);
	}
	
    if(SDL_AUDIO_ISINT(obtained_spec.format)) {
        std::cout<<"Is int"<<std::endl;
    }
    std::cout<<"Bitsize: "<<SDL_AUDIO_BITSIZE(obtained_spec.format)<<std::endl;

	/* Start playing */
	SDL_PauseAudioDevice(device, 0);

	// play for 3 seconds
	while ( sample_counter < obtained_spec.freq * 3 ) {
		SDL_Delay(100); 
	}
	
	// shut everything down
	SDL_CloseAudioDevice(device);
}


constexpr double pi = 3.141592653589793238;


template<typename SAMPLE_TYPE>
void writeSine(void* audioBuffer_Raw,
        uint32_t bufferBytes,
        uint32_t sampleRate,
        double cyclesPerSec,
        uint32_t startSample = 0)   // Can be used to offset the phase of the sine wave
{
    const uint32_t numSamples = bufferBytes / sizeof(SAMPLE_TYPE);
    auto * audioBuffer = reinterpret_cast<SAMPLE_TYPE*>(audioBuffer_Raw);
    const double cyclesPerSample = cyclesPerSec / static_cast<double>(sampleRate);
    const double factor = cyclesPerSample * 2 * pi; // Not particularly sure what to call this constant
    const double phaseOffset = factor * startSample;
    for(uint32_t i = 0; i < numSamples; ++i) {
        const double normal = sin(i * factor + phaseOffset);
        audioBuffer[i] = normal * static_cast<double>(std::numeric_limits<SAMPLE_TYPE>::max());
    }
}

typedef int16_t SampleType;


// audio callback function
// here you have to copy the data of your audio buffer into the
// requesting audio buffer (stream)
// you should only copy as much as the requested length (len)
void my_audio_callback(void *userdata, Uint8 *stream, int len) {
    auto * sample_counter = reinterpret_cast<unsigned int*>(userdata);
    auto bufferLen = len / sizeof(SampleType);

    writeSine<SampleType>(stream, len, 44100, 440, *sample_counter);

	*sample_counter += bufferLen;
}