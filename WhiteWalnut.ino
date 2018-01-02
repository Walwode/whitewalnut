#include "PlantCircuit.h"
#include "Esp8266-01.h"
#include "StensTimer.h"

StensTimer* stensTimer;

void evolveEcoSystem()
{
	Esp8266* esp8266 = new Esp8266();

	PlantCircuit* plant1 = new PlantCircuit("Plant Left");
	plant1->humiditySetup(12, A0, 60000, 525);
	// plant1->humiditySetup(12, A0, 20000, 525);
	// plant1->irrigationSetup(11, 10000, 525, 6 * 60 * 60 * 1000);
	plant1->wifiSetup(esp8266);
	plant1->checkHumidity();
	// plant1->lightSetup(7, A2, 1000, 60);

	// PlantCircuit* plant2 = new PlantCircuit("Plant Right");
	// plant2->humiditySetup(10, A1, 20000, 525);
	// plant2->irrigationSetup(9, 3000, 600, 6 * 60 * 60 * 1000);
	// plant2->wifiSetup(esp8266);
}

void setup()
{
	Serial.begin(9600);
	while (!Serial);
	
	stensTimer = StensTimer::getInstance();
	evolveEcoSystem();
}

void loop()
{
	stensTimer->run();
}
