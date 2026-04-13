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
}

void loop() {
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
