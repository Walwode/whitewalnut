#include "PlantCircuit.h"

#define CHECK_HUMIDITY_ACTION 1
#define STOP_IRRIGATION_ACTION 2

PlantCircuit::PlantCircuit() {}
PlantCircuit::PlantCircuit(String name)
{
	plantName = name;
	
	Serial.print("Created ");
	Serial.println(getName());
}
PlantCircuit::~PlantCircuit() {}

// >> general
String PlantCircuit::getName()
{
	return plantName;
}

void PlantCircuit::timerCallback(Timer* timer)
{
	// Serial.println("TimerCallback " + String(timer->getAction()) + " for " + getName());
	switch(timer->getAction()) {
		case CHECK_HUMIDITY_ACTION:
			checkHumidity();
			checkIrrigation();
			break;
		case STOP_IRRIGATION_ACTION:
			stopIrrigation();
			break;
		default:
			break;
	}
}
// << general

// >> humidity
void PlantCircuit::humiditySetup(byte vccPin, byte dataPin, long interval)
{
	humidityVccPin = vccPin;
	humidityDataPin = dataPin;

  StensTimer::getInstance()->setInterval(this, CHECK_HUMIDITY_ACTION, interval);
}

int PlantCircuit::checkHumidity()
{
	digitalWrite(humidityVccPin, HIGH);
	delay(1000); // TODO
	humidity = 1023 - analogRead(dataPin);
	digitalWrite(humidityVccPin, LOW);
	
	Serial.print("Check Humidity (");
	Serial.print(getName());
	Serial.print(")... ");
	Serial.println(String(getHumidity()));
	
	return getHumidity();
}

int PlantCircuit::getHumidity()
{
	return humidity;
}
// << humidity

// >> irrigation
void PlantCircuit::irrigationSetup(byte vccPin, long duration, bool automatic)
{
	irrigation = automatic;
	irrigationVccPin = vccPin;
	irrigationDuration = duration;
}

void PlantCircuit::checkIrrigation()
{
	if (irrigation && isDry() && !irrigating) startIrrigation();
}

void PlantCircuit::startIrrigation()
{
  if (!irrigating) {
    irrigating = true;
	digitalWrite(irrigationVccPin, HIGH);
    StensTimer::getInstance()->setTimer(this, STOP_IRRIGATION_ACTION, irrigationDuration);
	
    Serial.println("Start Irrigation (");
	Serial.print(getName());
	Serial.print(")... ");
	Serial.println(String(irrigationDuration));
  }
}

void PlantCircuit::stopIrrigation()
{
	irrigating = false;
	digitalWrite(irrigationVccPin, LOW);
	Serial.print("Stop Irrigation ("
	Serial.print(getName());
	Serial.println(")...");
}

bool PlantCircuit::isDry()
{
	return (humidity < 500);
}
// << irrigation
