/*
 * AudioInterface.h
 *
 *  Created on: Feb 14, 2021
 *      Author: lars
 */

#ifndef AUDIOINTERFACE_H_
#define AUDIOINTERFACE_H_
#include "IClockSource.h"
#include "IAudioModule.h"
#include "portaudio.h"
#include <string>


class AudioInterface : public IClockSource, public IAudioModule{
public:
	AudioInterface();
	virtual ~AudioInterface();
	static AudioInterface *Instance();

	float get() override;
	void Init(IAudioModule *lSrc, IAudioModule *rSrc);
	static int paTestCallback(const void* inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData);


private:


	static AudioInterface *instance;

	float m_out;
	bool m_running;
	int m_sample_rate;
	int m_frames_per_buffer;
	IAudioModule *m_lSrc;
	IAudioModule *m_rSrc;


};

class AudioException : public std::exception{
public:
	AudioException(std::string msg):_msg(msg){}
	virtual const char *what() const noexcept override{return _msg.c_str();}
private:
	std::string _msg;
};

#endif /* AUDIOINTERFACE_H_ */
