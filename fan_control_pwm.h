/**
 * Author:    Serhii Horelskyi
 * Created:   07.06.2020
 *
 * (c) Serhii Horelskyi
 **/

class Fan_PWM {
  private:
    int speed_state = 0;
    int current_pwm_speed = 0;
    int requested_pwm_speed = 0;
  public:
    bool debug = false;
    int speed_factor = 5;         // Voltage speed increasing, milliseconds
    int pwm_pin = 5;              // Port, default D5
    int pwm_speed_1 = 10;         // PWM value for speed #1
    int pwm_speed_2 = 255;        // PWM value for speed #2
    int pwm_speed_select = 0;     // Select speed beetween 1 and 2 or stop

    int init() {
      pinMode(pwm_pin, OUTPUT);   // init port mode
      analogWrite(pwm_pin, 0);    // set port out to LOW - turn OFF (default)
    }

    int run() {

      // If selected speed 1
      if (pwm_speed_select == 1) {
        if (speed_state != pwm_speed_select) {
          for (current_pwm_speed = 0; current_pwm_speed <= pwm_speed_1; current_pwm_speed++) {
            if (debug) {
              Serial.print("DEBUG (fan_control_pwm): FAN speed increase, raw PWM value: ");
              Serial.println(current_pwm_speed);
            }
            analogWrite(pwm_pin, current_pwm_speed);
            delay(speed_factor);
          }
          speed_state = pwm_speed_select;
        }
        else {
          if (debug) {
            Serial.print("DEBUG (fan_control_pwm): Update FAN speed to: ");
            Serial.println(pwm_speed_select);
          }
          analogWrite(pwm_pin, pwm_speed_1);
          current_pwm_speed = pwm_speed_1;
          speed_state = pwm_speed_select;
        }
      }

      // If selected speed 2
      else if (pwm_speed_select == 2) {
        if (speed_state != pwm_speed_select) {
          if (debug) {
            Serial.print("DEBUG (fan_control_pwm): Run FAN to speed: ");
            Serial.println(pwm_speed_select);
          }
          for (current_pwm_speed = current_pwm_speed; current_pwm_speed <= pwm_speed_2; current_pwm_speed++) {
            if (debug) {
              Serial.print("DEBUG (fan_control_pwm): FAN speed increase, raw PWM value: ");
              Serial.println(current_pwm_speed);
            }
            analogWrite(pwm_pin, current_pwm_speed);
            delay(speed_factor);
          }
          speed_state = pwm_speed_select;
        }
        else {
          if (debug) {
            Serial.print("DEBUG (fan_control_pwm): Update FAN speed to: ");
            Serial.println(pwm_speed_select);
          }
          analogWrite(pwm_pin, pwm_speed_2);
          current_pwm_speed = pwm_speed_2;
          speed_state = pwm_speed_select;
        }
      }

      // If selected speed 0
      else {
        if (debug) {
          Serial.println("DEBUG (fan_control_pwm): Stop FAN");
        }
        analogWrite(pwm_pin, 0);
        current_pwm_speed = 0;
        speed_state = pwm_speed_select;
      }
      if (debug) {
        Serial.print("DEBUG (fan_control_pwm): pwm_speed_select: ");
        Serial.println(pwm_speed_select);
        Serial.print("DEBUG (fan_control_pwm): speed_state: ");
        Serial.println(speed_state);
      }
      return 0;
    }

};
