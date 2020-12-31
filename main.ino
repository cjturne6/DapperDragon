#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"
#include "RTClib.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

RTC_DS3231 rtc;

DHT dht1(8, DHT22);
DHT dht2(9, DHT22);

float temp1;
float temp2;
float hum1;
float hum2;

int heatLamp = 2;
int uvbLamp = 3;
bool nighttime = 0;
bool tooHot = 0;
bool tooCold = 0;

DateTime now;

void setup() {
	Serial.begin(9600);
	
	dht1.begin();
	dht2.begin();

	if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
		Serial.println(F("SSD1306 allocation failed"));
		for(;;);
		}
	
	#ifndef ESP8266
		while (!Serial); // wait for serial port to connect. Needed for native USB
		#endif
	
	if (! rtc.begin()) {
    	Serial.println("Couldn't find RTC");
    	Serial.flush();
    	abort();
		}

	if (rtc.lostPower()) {
		Serial.println("RTC lost power, let's set the time!");
		rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
		}
  
	delay(2000);
	display.clearDisplay();
	display.setTextColor(WHITE);
	display.setTextSize(1);
}

void getTime() {
	now = rtc.now();
	if (now.hour()>=21 || now.hour()<8) nighttime = 1;
	if (now.hour()<21 && now.hour()>=8) nighttime = 0;
}

void switchControl() {
	if (nighttime==1 && tooCold==0) {
		digitalWrite(uvbLamp,0);
		digitalWrite(heatLamp,0);
	}
	
	else if (nighttime==0 && tooHot==0) {
		digitalWrite(uvbLamp,1);
		digitalWrite(heatLamp,1);
	}
	
	else if (nighttime==1 && tooCold==1) {
		digitalWrite(uvbLamp,0);
		digitalWrite(heatLamp,1);
	}
	
	else if (nighttime==0 && tooHot==0) {
		digitalWrite(uvbLamp,1);
		digitalWrite(heatLamp,1);
	}
	
	else if (nighttime==0 && tooHot==1) {
		digitalWrite(uvbLamp,1);
		digitalWrite(heatLamp,0);
	}
}

void checkTemp() {
	hum1 = dht1.readHumidity();
	temp1 = dht1.readTemperature(true);		// true means fahrenheit
	hum2 = dht2.readHumidity();
 	temp2 = dht2.readTemperature(true);		// true means fahrenheit

	// Check if any reads failed and exit early (to try again).
	if (isnan(hum1) || isnan(temp1)) Serial.println(F("Failed to read from sensor 1!"));
	if (isnan(hum2) || isnan(temp2)) Serial.println(F("Failed to read from sensor 2!"));
  
	if (nighttime==0) {
		if (temp1>100 || temp2>85) tooHot=1;
		else if (temp1<=100 || temp2<=85) tooHot=0;
		}
		
	if (nighttime==1) {
		if (((temp1+temp2)/2)<65) tooCold=1;
		else if (((temp1+temp2)/2)>=65) tooCold=0;
		}
	switchControl();
	
	/*
	Serial.print(F("Humidity 1: "));
	Serial.print(hum1);
	Serial.print(F("%  Temperature 1: "));
	Serial.print(temp1);
	Serial.print(F("°F "));

	Serial.print(F("Humidity 2: "));
	Serial.print(hum2);
	Serial.print(F("%  Temperature 2: "));
	Serial.print(temp2);
	Serial.println(F("°F"));
	*/
}

void displayTime() {
	display.setCursor(98,3);
	if (now.hour()<10) display.print(" ");
	display.print(now.hour(),DEC);
	display.print(":");
	if (now.minute()<10) display.print("0");
	display.print(now.minute(),DEC);

	return;
}

void displaySwitch() {
	display.clearDisplay();
	displayTime();
	display.setCursor(0,3);
	display.print("POWER");
  
	display.setCursor(0, 16);
	display.println("Heat lamp:");
	display.setCursor(90,16);
	if (digitalRead(heatLamp)==HIGH) display.print("ON");
	if (digitalRead(heatLamp)==LOW) display.print("OFF");
  
	display.setCursor(0, 28);
	display.println("UVB lamp:");
	display.setCursor(90,28);
	if (digitalRead(uvbLamp)==HIGH) display.print("ON");
	if (digitalRead(uvbLamp)==LOW) display.print("OFF");
  
	display.setCursor(0, 40);
	display.println("Reserved:");
	display.setCursor(90,40);
	display.print("N/A");
	
	display.setCursor(0, 52);
	display.println("Reserved:");
	display.setCursor(90,52);
	display.print("N/A");
  
	display.display();
}

void displayTemp() {
	display.clearDisplay();
	displayTime();
	display.setCursor(0,3);
	display.print("TEMPERATURES");
  
	display.setCursor(0, 16);
	display.println("Temp #1:");
	display.setCursor(90,16);
	display.println(temp1);
	display.setCursor(122, 16);
	display.println("F");
  
	display.setCursor(0, 36);
	display.println("Temp #2:");
	display.setCursor(90,36);
	display.print(temp2);
	display.setCursor(122,36);
	display.print("F");
  
	display.setCursor(0, 56);
	display.println("Temp (avg):");
	display.setCursor(90,56);
	display.print((temp1+temp2)/2);
	display.setCursor(122,56);
	display.print("F");
  
	display.display();
}

void displayHum() {
	display.clearDisplay();
	displayTime();
	display.setCursor(0,3);
	display.print("HUMIDITY");
  
	display.setCursor(0, 16);
	display.println("RH #1:");
	display.setCursor(90,16);
	display.println(hum1);
	display.setCursor(122, 16);
	display.println("%");
  
	display.setCursor(0, 36);
	display.println("RH #2:");
	display.setCursor(90,36);
	display.print(hum2);
	display.setCursor(122,36);
	display.print("%");
  
	display.setCursor(0, 56);
	display.println("RH (avg):");
	display.setCursor(90,56);
	display.print((hum1+hum2)/2);
	display.setCursor(122,56);
	display.print("%");
  
	display.display();
}

void loop() {
	getTime();
	checkTemp();
	displaySwitch();
	delay(5000);
	getTime();
	checkTemp();
	displayHum();
	delay(5000);
	getTime();
	now = rtc.now();
	checkTemp();
	displayTemp();
	delay(5000);
	getTime();
	}
