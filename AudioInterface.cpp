/*
 * AudioInterface.cpp
 *
 *  Created on: Feb 14, 2021
 *      Author: lars
 */

#include "AudioInterface.h"
#include "portaudio.h"
#include <iostream>
#include <assert.h>

AudioInterface *AudioInterface::instance = nullptr;
#define SAMPLE_RATE 44100

typedef struct {
		float leftPhase;
		float rightPhase;
}paTestData;

AudioInterface::AudioInterface() {
	// TODO Auto-generated constructor stub
	m_out = 0.0f;
	m_running = false;
	m_sample_rate = SAMPLE_RATE;
	m_freq = SAMPLE_RATE; // @suppress("Symbol is not resolved")
	m_frames_per_buffer = 32;
	m_lSrc = nullptr;
	m_rSrc = nullptr;
}

AudioInterface::~AudioInterface() {
	// TODO Auto-generated destructor stub
}

AudioInterface *AudioInterface::Instance(){
	if(instance == nullptr){
		instance = new AudioInterface();
	}
	return instance;
}

float AudioInterface::get(){
	return m_out;
}

paTestData data;
PaStream *stream;

void AudioInterface::Init(IAudioModule *lSrc, IAudioModule *rSrc){
	m_lSrc = lSrc;
	m_rSrc = rSrc;
	try
	{
		PaError err = Pa_Initialize();
		if(err != paNoError){
			throw AudioException(Pa_GetErrorText(err));
		}

		err = Pa_OpenDefaultStream(
				&stream,		//Stream
				2,				//Number of input channels
				2,				//Number of output channels
				paFloat32,		//Data type
				m_sample_rate,	//Sample rate
				m_frames_per_buffer,			//frames per buffer
				AudioInterface::paTestCallback,	//Callback
				&data);			//data
		if(err != paNoError){
			throw AudioException( Pa_GetErrorText(err));
		}
		err = Pa_StartStream(stream);
		if(err != paNoError){
			throw AudioException(Pa_GetErrorText(err));
		}
		m_running = true;
	}catch(AudioException &e){
		std::cout << "Audio exception: " << e.what() << std::endl;
	}
}

int AudioInterface::paTestCallback(const void* inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData){
	assert(instance != nullptr);
	assert(instance->m_lSrc != nullptr);
	assert(instance->m_rSrc != nullptr);

	//paTestData *data = reinterpret_cast<paTestData*>(userData);
	float *out = reinterpret_cast<float*>(outputBuffer);
	const float *in = reinterpret_cast<const float*>(inputBuffer);

	for(unsigned long i=0;i<framesPerBuffer;i++ ){
		const float inleft = *(in++);
		const float inright = *(in++);

		instance->m_out = inleft + inright;
		instance->m_out =instance->m_out * 1.4f;
		instance->updateObservers();
		*(out++) = instance->m_lSrc->get();
		*(out++) = instance->m_rSrc->get();
	}

	return 0;
}

