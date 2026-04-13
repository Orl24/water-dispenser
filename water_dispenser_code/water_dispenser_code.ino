/* WIFI part */
#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPLaMBC74-X"
#define BLYNK_TEMPLATE_NAME "NodeMCU"
#define BLYNK_AUTH_TOKEN "YourAuthToken"

#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "YourAuthToken"; //maybe can get rid of this one

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Habets";
char pass[] = "pephabet1228";

/***************************/

int pin2 = 2; //Light
int pin5 = 5; //GPIO 5

double thirty_mins = 1800; 

/*********************************************/
// Adjust these only
double time_in_seconds = thirty_mins; //set how often to dispense water
double DISPENSE_LENGTH = 25; // 10 = 1 second - how long to dispense the water
// 20 = 5cc
// 30 = 15cc
/*********************************************/

double WHEN_TO_DISPENSE_SECONDS = time_in_seconds * 10; //10 = 1 second so need to * 10

double dispense_counter = 0;
double dispense_time_counter = 0;

double LIGHT_SWITCH_TIME = 10;
double light_counter_on = 0;
double light_counter_off = 0;

bool TEST_MOTOR = 0;
double DISPENSE_TIME = 2000;

void setup() {
  // put your setup code here, to run once:
  pinMode(pin2, OUTPUT);
  pinMode(pin5, OUTPUT);

  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();      //keeps your device connected to the Blynk IoT and processes everything in real time. helps reconnect.

  // val=1024-analogRead(yl69_pin);     // reads analogue value like the moisture sensor, then minus 1024 to invert the value to make it easier to understand
  // Serial.println(val);               // Sends the value to Serial Monitor
  //Blynk.virtualWrite(V1, (int)val);   // Sends the value to Blynk IoT app. V1 means virtual pin 1

  // put your main code here, to run repeatedly:
  if(TEST_MOTOR == 0)
  {
    //light on and off
    if(++light_counter_on <= LIGHT_SWITCH_TIME)
    {
      digitalWrite(pin2, LOW);  // ON - light
      if(light_counter_on == LIGHT_SWITCH_TIME)
        light_counter_off = 0;
    }
    else if(++light_counter_off <= LIGHT_SWITCH_TIME)
    {
      digitalWrite(pin2, HIGH);  // OFF - light
      if(light_counter_off == LIGHT_SWITCH_TIME)
        light_counter_on = 0;
    }

    // track dispense counter
    if(++dispense_counter >= WHEN_TO_DISPENSE_SECONDS)
    {
      // dispense water for a set period
      if(++dispense_time_counter <= DISPENSE_LENGTH)
      {
        digitalWrite(pin5, HIGH);  // ON - pump
      }
      else
      {
        digitalWrite(pin5, LOW);  // OFF - pump
        dispense_counter = 0;
        dispense_time_counter = 0;
      }
    }

    delay(100); // loop once every 0.1 second
  }
  else
  {
    // Turn OFF pump
    digitalWrite(pin2, HIGH);  // OFF - light
    digitalWrite(pin5, LOW);   // OFF - pump
    delay(DISPENSE_TIME);         // Wait time

  // Turn ON pump
    digitalWrite(pin2, LOW);   // ON - light
    digitalWrite(pin5, HIGH);  // ON - pump
    delay(DISPENSE_TIME);      // Run time
  }
}
