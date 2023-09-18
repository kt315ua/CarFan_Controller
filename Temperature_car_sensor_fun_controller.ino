/**
 * Author:    Serhii Horelskyi
 * Created:   08.12.2019
 *
 * (c) Serhii Horelskyi
 **/

#include "sensor_temp.h"
#include "fan_speed_switch.h"
#include "fan_control_pwm.h"
#include "fan_control_relay.h"

#define ARDUINO_NANO 1               // Comment if Used Arduino MINI

#ifdef ARDUINO_NANO
// I/O Port for NANO                           MINI    NANO
unsigned int io_port_1 = 2;                 // Digital: D2      D2
unsigned int io_port_2 = 3;                 // Digital: D3      D3
unsigned int io_port_3 = 7;                 // Analog:  A3      A7
unsigned int io_port_4 = 6;                 // Analog:  A2      A6
unsigned int io_port_5 = 5;                 // Analog:  A1      A5
unsigned int io_port_6 = 2;                 // Digital: D12     A2
#endif

#ifndef ARDUINO_NANO
// I/O Port for MINI                           MINI    NANO
unsigned int io_port_1 = 2;                 // Digital: D2      D2
unsigned int io_port_2 = 3;                 // Digital: D3      D3
unsigned int io_port_3 = 3;                 // Analog:  A3      A7
unsigned int io_port_4 = 2;                 // Analog:  A2      A6
unsigned int io_port_5 = 1;                 // Analog:  A1      A5
unsigned int io_port_6 = 12;                // Digital: D12     A2
#endif

Temperature_sensor engine_temp;
Fan_speed_switch fan_speed_select;
Fan_PWM fan_1_pwm;
Fan_PWM fan_2_pwm;
Fan_Relay fan_relay;


// Define temperature sensor cilibration
unsigned int R1 = 430;         // STD nominal
unsigned int R_Sensor = 3520;    // Default basic temp: 20'C
unsigned int dt_window = 5;      // Delta temp for each temp reading. Validate sensor data to avoid connection issues

// Define fan speed temperature triggers
/*
   VAZ 2101-07, read from injector cars
   T ON, 'C:  101
   T OFF, 'C: 98
*/

float temp_speed_1_start = 100;
float temp_speed_1_stop  = 98;
float temp_speed_2_start = 101;
float temp_speed_2_stop  = 99;
float temp = 20;

unsigned int fan_speed = 0;
unsigned int fan_min_time = 30;  // minimal timer for turning on FAN, seconds

unsigned int relay_speed_1_pin = 4;         // D4
unsigned int relay_speed_2_pin = 7;         // D7
unsigned int fan_1_pwm_pin = 5;             // D5
unsigned int fan_2_pwm_pin = 6;             // D6
unsigned int ledPin = 13;                   // Arduino LED

// Output Levels for FAN PWM
unsigned int fan_1_speed_factor = 5;   // ms
unsigned int fan_1_pwm_speed_1 = 92;
unsigned int fan_1_pwm_speed_2 = 255;
unsigned int fan_2_speed_factor = 5;   // ms
unsigned int fan_2_pwm_speed_1 = 127;
unsigned int fan_2_pwm_speed_2 = 255;

/*
  void siren() {     //This function produces the 3rd siren (AMBULANCE sound).
  noTone(speakerOut);
  for(int i=800;i<1200;i+=8){
      tone(speakerOut,i);
      delay(5);
      }
  noTone(speakerOut);

  }
*/

int start_fan(int fan_speed) {
  // Manage PIN that indicate global FAN state - is it ON or OFF. Speed have no matter
  if (fan_speed != 0) {
    digitalWrite(ledPin, true);

  }
  else {
    digitalWrite(ledPin, false);
  }

  // Manage 1st SPEED PIN that indicate FAN 1st SPEED only
  if (fan_speed == 1) {
    fan_relay.relay_speed_select = 1;
    fan_relay.run();
    fan_1_pwm.pwm_speed_select = 1;
    fan_1_pwm.run();
    fan_2_pwm.pwm_speed_select = 1;
    fan_2_pwm.run();
  }
  // Manage 2nd SPEED PIN that indicate FAN 2nd SPEED only
  else if (fan_speed == 2) {
    fan_relay.relay_speed_select = 2;
    fan_relay.run();
    fan_1_pwm.pwm_speed_select = 2;
    fan_1_pwm.run();
    fan_2_pwm.pwm_speed_select = 2;
    fan_2_pwm.run();
  }
  // Stop FANs
  else {
    fan_relay.relay_speed_select = 0;
    fan_relay.run();
    fan_1_pwm.pwm_speed_select = 0;
    fan_1_pwm.run();
    fan_2_pwm.pwm_speed_select = 0;
    fan_2_pwm.run();
  }

  return 0;
}


void setup()
{
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, false);
  Serial.begin(9600);

  engine_temp.R1 = R1;  // STD nominal
  engine_temp.R_Sensor = R_Sensor;  // Default normal operate
  engine_temp.dt_window = dt_window;

  fan_speed_select.temp_speed_1_start = temp_speed_1_start;
  fan_speed_select.temp_speed_1_stop  = temp_speed_1_stop;
  fan_speed_select.temp_speed_2_start = temp_speed_2_start;
  fan_speed_select.temp_speed_2_stop  = temp_speed_2_stop;
  fan_speed_select.fan_min_time = fan_min_time;  // minimal timer for turning on FAN, seconds

  fan_relay.relay_speed_1_pin = relay_speed_1_pin;
  fan_relay.relay_speed_2_pin = relay_speed_2_pin;
  fan_relay.init();

  fan_1_pwm.pwm_pin = fan_1_pwm_pin;
  fan_1_pwm.pwm_speed_1 = fan_1_pwm_speed_1;
  fan_1_pwm.pwm_speed_2 = fan_1_pwm_speed_2;
  fan_1_pwm.speed_factor = fan_1_speed_factor;
  fan_1_pwm.init();

  fan_2_pwm.pwm_pin = fan_2_pwm_pin;
  fan_2_pwm.pwm_speed_1 = fan_2_pwm_speed_1;
  fan_2_pwm.pwm_speed_2 = fan_2_pwm_speed_2;
  fan_2_pwm.speed_factor = fan_2_speed_factor;
  fan_2_pwm.init();

}

void loop()
{
  temp = engine_temp.get_temp();
  Serial.print("Main (I) - Current Temperatue: ");
  Serial.println(temp);

  fan_speed_select.temp = temp;
  fan_speed = fan_speed_select.get_fan_speed();
  Serial.print("Main (I) - Select FAN speed: ");
  Serial.println(fan_speed);

  /*
     need use 2 pins for 2 relays
     than
     use interrupts to start counter 30 sec, that will be updated for speed statuses
     FAN will be stopped by counter interrupt
  */

  start_fan(fan_speed);

  delay(1000);
}
