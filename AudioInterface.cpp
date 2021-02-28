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

AudioInterface *AudioInterface::_instance = nullptr;
#define SAMPLE_RATE 44100

typedef struct {
		float leftPhase;
		float rightPhase;
}paTestData;

AudioInterface::AudioInterface() {
	// TODO Auto-generated constructor stub
	_out = 0.0f;
	_running = false;
	_sample_rate = SAMPLE_RATE;
	m_freq = SAMPLE_RATE; // @suppress("Symbol is not resolved")
	_frames_per_buffer = 32;
	_lSrc = nullptr;
	_rSrc = nullptr;
}

AudioInterface::~AudioInterface() {
	// TODO Auto-generated destructor stub
}

AudioInterface *AudioInterface::Instance(){
	if(_instance == nullptr){
		_instance = new AudioInterface();
	}
	return _instance;
}

float AudioInterface::get(){
	return _out;
}

paTestData data;
PaStream *stream;

void AudioInterface::Init(IAudioModule *lSrc, IAudioModule *rSrc){
	_lSrc = lSrc;
	_rSrc = rSrc;
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
				_sample_rate,	//Sample rate
				_frames_per_buffer,			//frames per buffer
				AudioInterface::paTestCallback,	//Callback
				&data);			//data
		if(err != paNoError){
			throw AudioException( Pa_GetErrorText(err));
		}
		err = Pa_StartStream(stream);
		if(err != paNoError){
			throw AudioException(Pa_GetErrorText(err));
		}
		_running = true;
	}catch(AudioException &e){
		std::cout << "Audio exception: " << e.what() << std::endl;
	}
}

int AudioInterface::paTestCallback(const void* inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData){
	assert(_instance != nullptr);
	assert(_instance->_lSrc != nullptr);
	assert(_instance->_rSrc != nullptr);

	//paTestData *data = reinterpret_cast<paTestData*>(userData);
	float *out = reinterpret_cast<float*>(outputBuffer);
	const float *in = reinterpret_cast<const float*>(inputBuffer);

	for(unsigned long i=0;i<framesPerBuffer;i++ ){
		const float inleft = *(in++);
		const float inright = *(in++);

		_instance->_out = inleft + inright;
		_instance->_out = _instance->_out * 1.4f;
		_instance->updateObservers();
		*(out++) = _instance->_lSrc->get();
		*(out++) = _instance->_rSrc->get();
	}

	return 0;
}

