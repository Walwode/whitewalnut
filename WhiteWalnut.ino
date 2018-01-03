#include "PlantCircuit.h"
#include "Esp8266-01.h"
#include "StensTimer.h"

#define ESP8266_RX_PIN 3
#define ESP8266_TX_PIN 2
#define ESP8266_BAUD 9600

StensTimer* stensTimer;
SoftwareSerial espSerial(ESP8266_RX_PIN, ESP8266_TX_PIN);

void evolveEcoSystem()
{
	Esp8266* esp8266 = new Esp8266(&espSerial, ESP8266_BAUD, 5000);

	PlantCircuit* plant1 = new PlantCircuit("Plant Left");
	plant1->humiditySetup(12, A0, 15 * 60000);
	plant1->irrigationSetup(11, 10000, 550, 60 * 60000);
	plant1->wifiSetup(esp8266);
	plant1->checkHumidity();
	// plant1->lightSetup(7, A2, 1000, 60);

	PlantCircuit* plant2 = new PlantCircuit("Plant Right");
	plant2->humiditySetup(10, A1, 30 * 60000);
	plant2->irrigationSetup(9, 3000, 550, 60 * 60000);
	plant2->wifiSetup(esp8266);
	plant2->checkHumidity();
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
