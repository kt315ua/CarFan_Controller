/**
 * Author:    Serhii Horelskyi
 * Created:   08.12.2019
 *
 * (c) Serhii Horelskyi
 **/

#include "sensor_temp.h"
#include "fan_speed_switch.h"
#include <avr/power.h>

#define DEBUG true
#define ARDUINO_NANO 1               // Comment if Used Arduino MINI

#ifdef ARDUINO_NANO
  // I/O Port (different for NANO or MINI):      MINI    NANO       
  int io_port_1 = 2;                 // Digital: D2      D2
  int io_port_2 = 3;                 // Digital: D3      D2
  int io_port_3 = 7;                 // Analog:  A3      A7
  int io_port_4 = 6;                 // Analog:  A2      A6
  int io_port_5 = 5;                 // Analog:  A1      A5
  int io_port_6 = 2;                 // Digital: D12     A2
#endif

#ifndef ARDUINO_NANO
  // I/O Port (different for NANO or MINI):      MINI    NANO       
  int io_port_1 = 2;                 // Digital: D2      D2
  int io_port_2 = 3;                 // Digital: D3      D2
  int io_port_3 = 3;                 // Analog:  A3      A7
  int io_port_4 = 2;                 // Analog:  A2      A6
  int io_port_5 = 1;                 // Analog:  A1      A5
  int io_port_6 = 12;                // Digital: D12     A2
#endif

Temperature_sensor engine_temp;  
Fan_speed_switch fan_speed_select;


// Define temperature sensor cilibration
float R1 = 430.0;         // STD nominal
float R_Sensor = 3520;    // Default basic temp: 20'C
float dt_window = 5;

// Define fan speed temperature triggers
/*
 * VAZ 2101-07, read from injector cars
 * T ON, 'C:  101
 * T OFF, 'C: 98
*/
float temp_speed_1_start = 100;
float temp_speed_1_stop  = 98;
float temp_speed_2_start = 101;
float temp_speed_2_stop  = 99;
float sensor_cut_detect = -40;
unsigned int fan_min_time = 30;  // minimal timer for turning on FAN, seconds

int relay_speed_1_pin = 4;         // D4
int relay_speed_2_pin = 7;         // D7
int fan_1_pin = 5;                 // D5
int fan_2_pin = 6;                 // D6


float temp = 20;
unsigned int fan_speed = 0;

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

int start_fan(int fan_speed){
  // Manage PIN that indicate global FAN state - is it ON or OFF. Speed have no matter
  if (fan_speed != 0){
    digitalWrite(fan_1_pin, true);
    digitalWrite(fan_2_pin, true);
  }
  else{
    digitalWrite(fan_1_pin, false);
    digitalWrite(fan_2_pin, false);
  }

  // Manage 1st SPEED PIN that indicate FAN 1st SPEED only
  if (fan_speed == 1){
    digitalWrite(relay_speed_1_pin, true);
  }
  else{
    digitalWrite(relay_speed_1_pin, false);
  }

  // Manage 2nd SPEED PIN that indicate FAN 2nd SPEED only
  if (fan_speed == 2){
    digitalWrite(relay_speed_2_pin, true);
  }
  else{
    digitalWrite(relay_speed_2_pin, false);
  }
   
  return 0;
  }


void setup()
{
  pinMode(relay_speed_1_pin, OUTPUT);
  pinMode(relay_speed_2_pin, OUTPUT);
  pinMode(fan_1_pin, OUTPUT);
  pinMode(fan_2_pin, OUTPUT);
  digitalWrite(relay_speed_1_pin, false);
  digitalWrite(relay_speed_2_pin, false);
  digitalWrite(fan_1_pin, false);
  digitalWrite(fan_2_pin, false);
  Serial.begin(9600);
    
  engine_temp.debug = DEBUG;
  engine_temp.R1 = R1;  // STD nominal
  engine_temp.R_Sensor = R_Sensor;  // Default normal operate
  engine_temp.dt_window = dt_window;

  fan_speed_select.debug = DEBUG;
  fan_speed_select.temp_speed_1_start = temp_speed_1_start;
  fan_speed_select.temp_speed_1_stop  = temp_speed_1_stop;
  fan_speed_select.temp_speed_2_start = temp_speed_2_start;
  fan_speed_select.temp_speed_2_stop  = temp_speed_2_stop;
  fan_speed_select.fan_min_time = fan_min_time;  // minimal timer for turning on FAN, seconds
  fan_speed_select.sensor_cut = sensor_cut_detect;
}

void loop()
{
    temp = engine_temp.get_temp();
    Serial.print("INFO (MAIN): Current Temperatue: ");
    Serial.println(temp);    

    fan_speed_select.temp = temp;
    fan_speed = fan_speed_select.get_fan_speed();
    Serial.print("INFO (MAIN): Select FAN speed: ");
    Serial.println(fan_speed);  

    /*
     * need use 2 pins for 2 relays
     * than
     * use interrupts to start counter 30 sec, that will be updated for speed statuses
     * FAN will be stopped by counter interrupt 
     */

    start_fan(fan_speed);
  
    delay(1000);   
}
