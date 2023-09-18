/**
 * Author:    Serhii Horelskyi
 * Created:   07.06.2020
 *
 * (c) Serhii Horelskyi
 **/

class Fan_PWM {
  private:
    int current_pwm_speed = 0;
    int requested_pwm_speed = 0;
  public:
    bool debug = false;
    unsigned int speed_factor = 5;         // Voltage speed increasing, milliseconds
    unsigned int pwm_pin = 5;              // Port, default D5
    unsigned int pwm_speed_1 = 10;         // PWM value for speed #1
    unsigned int pwm_speed_2 = 255;        // PWM value for speed #2
    unsigned int pwm_speed_select = 0;     // Select speed beetween 1 and 2 or stop

    int init() {
      pinMode(pwm_pin, OUTPUT);   // init port mode
      analogWrite(pwm_pin, 0);    // set port out to LOW - turn OFF (default)
    }

    int run() {
      // Define speed FAN direction
      Serial.print("PWM control (I) - Current speed: ");
      Serial.println(current_pwm_speed);
      Serial.print("PWM control (I) - Requested speed: ");
      Serial.println(pwm_speed_select);

      // speed 1
      if (pwm_speed_select == 1) {
        Serial.print("PWM control (I) - PWM 1st speed, raw: ");
        Serial.println(pwm_speed_1);
        requested_pwm_speed = pwm_speed_1;
      }
      // speed 2
      else if (pwm_speed_select == 2) {

        Serial.print("PWM control (I) - PWM 2nd speed, raw: ");
        Serial.println(pwm_speed_2);
        requested_pwm_speed = pwm_speed_2;
      }
      // stop
      else {
        Serial.println("PWM control (I) - PWM stop: 0");
        requested_pwm_speed = 0;
      }

      // Increase FAN speed
      if (current_pwm_speed < requested_pwm_speed) {
        for (; current_pwm_speed <= requested_pwm_speed; current_pwm_speed++) {
          Serial.print("PWM control (I) - Increase speed: ");
          Serial.println(current_pwm_speed);
          analogWrite(pwm_pin, current_pwm_speed);
          delay(speed_factor);
        }
        current_pwm_speed = requested_pwm_speed;
      }
      // Decrease FAN speed
      else if (current_pwm_speed > requested_pwm_speed) {
        for (; current_pwm_speed >= requested_pwm_speed; current_pwm_speed--) {
          Serial.print("PWM control (I) - Decrease speed: ");
          Serial.println(current_pwm_speed);
          analogWrite(pwm_pin, current_pwm_speed);
          delay(speed_factor);
        }
        current_pwm_speed = requested_pwm_speed;
      }
      // Do nothing
      else {
          Serial.println("PWM control (I) - No need to change speed");
      }
      return 0;
    }
};
