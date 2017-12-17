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
  int humidityThreshold;
  byte humidityVccPin;
  byte humidityDataPin;
  
  // irrigation
  bool irrigation = false;
  bool irrigating = false;
  byte irrigationVccPin;
  long irrigationDuration;

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
    void humiditySetup(byte vccPin, byte dataPin, long interval, int threshold);
    int checkHumidity();
    int getHumidity();
  
    // irrigation
    void irrigationSetup(byte vccPin, long duration, bool automatic);
    int autoIrrigation();
    bool checkIrrigation();
    void startIrrigation();
    void stopIrrigation();
    bool isDry();
	
	// light
	void lightSetup(byte vccPin, byte dataPin, long interval, int threshold);
	void checkBrightness();
	void switchLightOn();
	void switchLightOff();
};

#endif
