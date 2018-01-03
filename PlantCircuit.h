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
	int irrigationHumidity; // 525 for 1 second preenergization
	long irrigationTimeLock;
	unsigned long nextIrrigationTime = 0;

	// internet
	Esp8266* esp8266;
	boolean internet = false;
	
	// light
	int lightThreshold;
	byte photoDataPin;
	byte lightVccPin;

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
		void irrigationSetup(byte vccPin, long duration);
		void irrigationSetup(byte vccPin, long duration, int humidityThreshold, long reIrrigationTimeLock);
		int autoIrrigation();
		bool checkIrrigation();
		void startIrrigation();
		void stopIrrigation();
		bool isDry();
		bool isBeyondIrrigationLock();
		
		// internet
		void wifiSetup(Esp8266* esp8266);
		bool checkEspConnection();
		void logToApi(String action, String result);
	
		// light
		void lightSetup(byte vccPin, byte dataPin, long interval, int threshold = 25);
		void checkBrightness();
		void switchLightOn();
		void switchLightOff();
};

#endif
