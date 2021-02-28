/*
 * guitareffect.cpp
 *
 *  Created on: Feb 13, 2021
 *      Author: lars
 */
#include "SimpleDelay.h"
#include "AudioInterface.h"
#include <unistd.h>
#include "Amp.h"
#include "SineOsc.h"
#include "Chorus.h"
#include "RandomGenerator.h"
#include "SlowClock.h"
#include "SawOsc.h"
#include "Distortion.h"
#include "Envelope.h"
#include "unistd.h"
#include "Filter.h"
#include "MeanTrigger.h"

int main(){
	//Auto wah
	//input -> amp ------------------>filter--> amp --> output
	//          |                       |
	//          -> trigger -> envelope --
	Amp pregain;
	pregain.SetSource(AudioInterface::Instance());
	pregain.SetLevel(3.0f);

	MeanTrigger trigger(AudioInterface::Instance());
	trigger.setSource(&pregain);
	trigger.setLevel(0.2f, 0.02f);


	Envelope env(AudioInterface::Instance(), 0.1f, 1.0f, 0.8f, 1.0f);
	trigger.setActionOn([&]()->void{env.KeyDown();});
	trigger.setActionOff([&]()->void{env.KeyUp();});

	Filter filter(AudioInterface::Instance(), Filter::FilterType::LOWPASS);
	filter.setCutoff(5000.0f);
	filter.setSource(&pregain);
	filter.setMod(&env, 0.0f, 1.0f, 400.0f);

	Amp master;
	master.SetSource(&filter);
	master.SetLevel(1.0f);



	AudioInterface::Instance()->Init(&master, &master);

	while(true){
		usleep(1000000);
	}

	return 0;
}
