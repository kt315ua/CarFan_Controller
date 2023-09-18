/**
 * Author:    Serhii Horelskyi
 * Created:   07.06.2020
 *
 * (c) Serhii Horelskyi
 **/

class Fan_Relay {
  private:
    int relay_1_state = 0;
    int relay_2_state = 0;
  public:
    bool debug = false;
    int relay_speed_1_pin = 6;              // D6
    int relay_speed_2_pin = 7;              // D7
    int relay_speed_select = 0;     // Select speed beetween 1 and 2 or stop

    int init() {
      pinMode(relay_speed_1_pin, OUTPUT);
      pinMode(relay_speed_2_pin, OUTPUT);
      digitalWrite(relay_speed_1_pin, false);
      digitalWrite(relay_speed_2_pin, false);
    }

    int run() {
      if (debug) {
        Serial.print("DEBUG (fan_control_relay): relay_speed_select: ");
        Serial.println(relay_speed_select);
        Serial.print("DEBUG (fan_control_relay): relay_1_state: ");
        Serial.println(relay_1_state);
        Serial.print("DEBUG (fan_control_relay): relay_2_state: ");
        Serial.println(relay_2_state);
      }

      // If selected speed 1
      if (relay_speed_select == 1) {
        digitalWrite(relay_speed_1_pin, true);
        relay_1_state = 1;
        digitalWrite(relay_speed_2_pin, false);
        relay_2_state = 0;
      }
      else if (relay_speed_select == 2) {
        digitalWrite(relay_speed_1_pin, false);
        relay_1_state = 0;
        digitalWrite(relay_speed_2_pin, true);
        relay_2_state = 1;
      }
      else if (relay_speed_select == 3) {
        digitalWrite(relay_speed_1_pin, true);
        relay_1_state = 1;
        digitalWrite(relay_speed_2_pin, true);
        relay_2_state = 1;
      }
      else {
        digitalWrite(relay_speed_1_pin, false);
        relay_1_state = 0;
        digitalWrite(relay_speed_2_pin, false);
        relay_2_state = 0;
      }
      if (debug) {
        Serial.print("DEBUG (fan_control_relay): relay_speed_select: ");
        Serial.println(relay_speed_select);
        Serial.print("DEBUG (fan_control_relay): relay_1_state: ");
        Serial.println(relay_1_state);
        Serial.print("DEBUG (fan_control_relay): relay_2_state: ");
        Serial.println(relay_2_state);
      }
    }
};
