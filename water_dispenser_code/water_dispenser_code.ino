/*EEPROM part*/
#include <EEPROM.h>
/* WIFI part */
#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL6TvrzGckA"
#define BLYNK_TEMPLATE_NAME "Chubby Water Dispensor"
#define BLYNK_AUTH_TOKEN "Q1LbzSd-v4ZbzvICB2O6DJI6WIY3_F_I"

#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = BLYNK_AUTH_TOKEN; //maybe can get rid of this one

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Habets";
char pass[] = "pephabet1228";

/***************************/
#define EEPROM_MAGIC 12345
/*********************************************/
//COMMUNICATION
int FIVE_SECONDS = 50;
/*********************************************/

int pin2 = 2; //Light
int pin5 = 5; //GPIO 5

/*********************************************/
// Adjust these only
int thirty_mins = 1800; 
int time_in_seconds = thirty_mins; //set how often to dispense water
int DISPENSE_LENGTH = 25; // 10 = 1 second - how long to dispense the water
// 20 = 5cc
// 30 = 15cc
/*********************************************/
int WHEN_TO_DISPENSE_SECONDS = time_in_seconds * 10; //10 = 1 second so need to * 10

int DISPENSE_LENGTH_OLD = 0;
int WHEN_TO_DISPENSE_SECONDS_OLD = 0;

int dispense_counter = 0;
int dispense_time_counter = 0;
int remaining_time = 0;
int communicate_remaining_time_to_dispense = 0;

int LIGHT_SWITCH_TIME = 10;
int light_counter_on = 0;
int light_counter_off = 0;

int DISPENSE_TIME = 2000;
int dispenses_triggered = 0;

int save_settings_counter = 0;
int SAVE_SETTINGS_FREQUENCY = 600; // check to save once every 60 seconds

//flags
bool TEST_MOTOR = 0;
bool V2_FLAG = 1; // switch (enable/disable function)
bool V3_FLAG = 0; // dispense now

void setup() {
  // put your setup code here, to run once:
  pinMode(pin2, OUTPUT);
  pinMode(pin5, OUTPUT);

  // Debug console
  Serial.begin(9600);
  delay(3000);
  // EEPROM
  EEPROM.begin(512);
  
  // debugging
  // int test1, test2;
  // EEPROM.get(0, test1);
  // EEPROM.get(10, test2);

  // Serial.println("Address 0 and 10 :");
  // Serial.println(test1);
  // Serial.println(test2);

  // check if first time getting memory
  int magic;
  EEPROM.get(100, magic);
  if (magic != EEPROM_MAGIC)
  {
    saveSettings();
    EEPROM.put(100, EEPROM_MAGIC);
  }
  else
  {
    loadSettings();
    DISPENSE_LENGTH_OLD = DISPENSE_LENGTH;
    WHEN_TO_DISPENSE_SECONDS_OLD = WHEN_TO_DISPENSE_SECONDS;
  }

  Blynk.begin(auth, ssid, pass);
}

void dispense_water_function() {
  // dispense water for a set period
  if(++dispense_time_counter <= DISPENSE_LENGTH)
  {
    digitalWrite(pin5, HIGH);  // ON - pump
  }
  else
  {
    digitalWrite(pin5, LOW);  // OFF - pump
    if(V3_FLAG == 1)
    {
      V3_FLAG = 0; // reset dispense now flag
      dispense_time_counter = 0;
    }
    else
    {
      dispense_counter = 0;
      dispense_time_counter = 0;
    }
    Blynk.virtualWrite(V1, int(++dispenses_triggered));
  }
}

void saveSettings() {
  if(DISPENSE_LENGTH_OLD != DISPENSE_LENGTH)
  {
    EEPROM.put(0, DISPENSE_LENGTH);           //Dispense duration
    DISPENSE_LENGTH_OLD = DISPENSE_LENGTH;
    EEPROM.commit();
  }
  if(WHEN_TO_DISPENSE_SECONDS_OLD != WHEN_TO_DISPENSE_SECONDS)
  {
    EEPROM.put(10, WHEN_TO_DISPENSE_SECONDS); //Frequency
    WHEN_TO_DISPENSE_SECONDS_OLD = WHEN_TO_DISPENSE_SECONDS;
    EEPROM.commit();
  }
}

void loadSettings() {
  EEPROM.get(0, DISPENSE_LENGTH);           //Dispense duration
  EEPROM.get(10, WHEN_TO_DISPENSE_SECONDS); //Frequency
}

void loop() {
  Blynk.run();      //keeps your device connected to the Blynk IoT and processes everything in real time. helps reconnect.
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

  // Check when to save
  if(++save_settings_counter >= SAVE_SETTINGS_FREQUENCY)
  {
    saveSettings();
    save_settings_counter = 0;
  }

  // dispense now
  if(V3_FLAG == 1)
  {
    dispense_water_function();
  }  

  // disable/enable function
  if(V2_FLAG == 1)
  {
    // track dispense counter
    if(++dispense_counter >= WHEN_TO_DISPENSE_SECONDS)
    {
      //function to dispense water
      dispense_water_function();
    }
    //calculate remaining time
    remaining_time = (WHEN_TO_DISPENSE_SECONDS - dispense_counter)/10;
    if(++communicate_remaining_time_to_dispense >= FIVE_SECONDS)
    {
      Blynk.virtualWrite(V5, int(remaining_time));
      communicate_remaining_time_to_dispense = 0;
    }
  }
  else
  {
    digitalWrite(pin5, LOW);  // OFF - pump
  }
  
  // test motor
  if (TEST_MOTOR == 1)
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
  delay(100); // loop once every 0.1 second
}

// Dispense frequency
BLYNK_WRITE(V0) {
    int value = param.asInt();
    if(value <= 1) value = 1; // minimum is every 10 seconds
    WHEN_TO_DISPENSE_SECONDS = value * 60 * 10; // *60 for seconds, *10 for count value in code
    dispense_counter = 0; // reset counter
}

// enable / disable function
BLYNK_WRITE(V2) {
  bool value = param.asInt();
  V2_FLAG = value;
}

// dispense now
BLYNK_WRITE(V3) {
    V3_FLAG = 1;
}

// Dispense duration
BLYNK_WRITE(V4) {
    int value = param.asInt();
    DISPENSE_LENGTH = value;
    //Blynk.virtualWrite(V6, int(DISPENSE_LENGTH)); // used for debugging
}