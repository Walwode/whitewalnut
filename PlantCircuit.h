#ifndef PLANTCIRCUIT_H
#define PLANTCIRCUIT_H

#include "Arduino.h"
#include "StensTimer.h"

class PlantCircuit : public IStensTimerListener
{
	// general
	String plantName;
	
	// humidity
	int humidity = 1024;
	byte humidityVccPin;
	byte humidityDataPin;
	
	// irrigation
	bool irrigation = false;
  bool irrigating = false;
	int irrigationVccPin;
	long irrigationDuration;

	
	public:
	
		// constructor
		PlantCircuit();
		PlantCircuit(String plantName);
	
		// destructor
		~PlantCircuit();

		// general
		String getName();
		void timerCallback(Timer* timer);
	
		// humidity
		void humiditySetup(byte vccPin, byte dataPin, long interval);
		int checkHumidity();
		int getHumidity();
	
		// irrigation
		void irrigationSetup(byte vccPin, long duration, bool automatic);
		int autoIrrigation();
		void checkIrrigation();
		void startIrrigation();
		void stopIrrigation();
		bool isDry();
};

#endif
