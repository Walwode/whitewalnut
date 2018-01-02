#include "PlantCircuit.h"
#include "Esp8266-01.h"
#include "StensTimer.h"

StensTimer* stensTimer;

/*
int shiftPin = 8; // SH_CP 74HC595
int storePin = 9; // ST_CP 74HC595
int dataPin = 10; // DS 74HC595
*/

void evolveEcoSystem()
{
	Esp8266* esp8266 = new Esp8266();

	PlantCircuit* plant1 = new PlantCircuit("Plant Left");
	plant1->humiditySetup(7, A1, 2000);
	plant1->irrigationSetup(8, 10000, 525, 6 * 60 * 60 * 1000);
	plant1->wifiSetup(esp8266);
	
	PlantCircuit* plant2 = new PlantCircuit("Plant Right");
	plant2->humiditySetup(9, A2, 2000);
	plant2->irrigationSetup(10, 15000, 600, 0);
	plant2->wifiSetup(esp8266);
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
