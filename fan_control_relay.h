/**
 * Author:    Serhii Horelskyi
 * Created:   07.06.2020
 *
 * (c) Serhii Horelskyi
 **/

class Fan_Relay {
  private:
    unsigned int relay_1_state = 0;
    unsigned int relay_2_state = 0;
  public:
    bool debug = false;
    unsigned int relay_speed_1_pin = 6;              // D6
    unsigned int relay_speed_2_pin = 7;              // D7
    unsigned int relay_speed_select = 0;     // Select speed beetween 1 and 2 or stop

    int init() {
      pinMode(relay_speed_1_pin, OUTPUT);
      pinMode(relay_speed_2_pin, OUTPUT);
      digitalWrite(relay_speed_1_pin, false);
      digitalWrite(relay_speed_2_pin, false);
    }

    int run() {
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
      Serial.print("Relay control (I) - Speed selected: ");
      Serial.println(relay_speed_select);
      Serial.print("Relay control (I) - Relay 1 state: ");
      Serial.println(relay_1_state);
      Serial.print("Relay control (I) - Relay 2 state: ");
      Serial.println(relay_2_state);
    }
};
