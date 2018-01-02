#ifndef PLANTCIRCUIT_H
#define PLANTCIRCUIT_H

#include "Arduino.h"
#include "StensTimer.h"
#include "Esp8266-01.h"

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
	int irrigationHumidity = 525; // 1 second preenergization
	int irrigationTimeLock = 6 * 60 * 60 * 1000; // 6 hours
	unsigned long nextIrrigationTime = 0;

	// internet
	Esp8266* esp8266;
	boolean internet = false;
	
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
		void irrigationSetup(byte vccPin, long duration, int humidityLimit, int reIrrigationTimeLock);
		int autoIrrigation();
		void checkIrrigation();
		void startIrrigation();
		void stopIrrigation();
		bool isDry();
		bool isBeyondIrrigationLock();
		
		// internet
		void wifiSetup(Esp8266* newEsp8266);
		void logToApi(String action, String result);
};

#endif
