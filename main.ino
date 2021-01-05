#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"
#include "RTClib.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define heatLamp (int)2
#define uvbLamp (int)3

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

RTC_DS3231 rtc; // creates rtc instance

DHT dht1(8, DHT22); DHT dht2(9, DHT22); // creates sensor instance

int temp1; int temp2; int hum1; int hum2; // declares variables to store temperature & humidity data

DateTime now; // creates DateTime instance for use with rtc

void setup() {
	Serial.begin(9600);
	pinMode(heatLamp,OUTPUT);
	pinMode(uvbLamp,OUTPUT);
	dht1.begin();
	dht2.begin();

	// serial troubleshooting for display.
	if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
		Serial.println(F("SSD1306 allocation failed"));
		for(;;);
		}
	
	// serial troubleshooting for rtc. replace with display output in the future.
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
	
	// initializes both lamps as off
	digitalWrite(uvbLamp, HIGH); digitalWrite(heatLamp, HIGH);
}

void switchControl() {
  // If it is daytime, set lamps depending on temp.
	  if (now.hour()<21 && now.hour()>=8) {
    if (temp1>100 || temp2>85) { digitalWrite(uvbLamp,HIGH); digitalWrite(2,LOW); }
	  else { digitalWrite(uvbLamp,LOW); digitalWrite(2,LOW); }
  }

  // If it is nighttime, set lamps depending on temp.
	else {
    if (((temp1+temp2)/2)<65) {digitalWrite(heatLamp,LOW); digitalWrite(uvbLamp,HIGH); }
		else { digitalWrite(uvbLamp, HIGH); digitalWrite(heatLamp, HIGH);}
	}
}

void getData() {
	now = rtc.now();
	hum1 = dht1.readHumidity(); temp1 = dht1.readTemperature(true);		// true means fahrenheit
	hum2 = dht2.readHumidity(); temp2 = dht2.readTemperature(true);		// true means fahrenheit
}

void displayTime() {
	display.setCursor(98,3);
	if (now.hour()<10) display.print(" "); // ensures proper number placement
	display.print(now.hour(),DEC);
	display.print(":");
	if (now.minute()<10) display.print("0"); // ensures proper number placement
	display.print(now.minute(),DEC);
}

void displaySwitch() {
  Serial.println("Now displaying power");
  
	display.clearDisplay();
	displayTime();
  
	display.setCursor(0,3);
	display.print("POWER");
  
	display.setCursor(0, 16);
	display.println("Heat lamp:");
	display.setCursor(90,16);
	if (digitalRead(2)==LOW) display.print("ON");
	else display.print("OFF");
  
	display.setCursor(0, 28);
	display.println("UVB lamp:");
	display.setCursor(90,28);
	if (digitalRead(uvbLamp)==LOW) display.print("ON");
	else display.print("OFF");
  
	display.display();
  delay(5000);
}

void displayTemp() {
  Serial.println("Now displaying temperature");
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
  delay(5000);
}

void displayHum() {
  Serial.println("Now displaying humidity");
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
  delay(5000);
}

void loop() {
	getData(); displaySwitch();
	getData(); displayHum();
	getData(); displayTemp();
    switchControl();
	}
