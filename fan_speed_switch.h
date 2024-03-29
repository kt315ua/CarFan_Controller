/**
 * Author:    Serhii Horelskyi
 * Created:   06.06.2020
 *
 * (c) Serhii Horelskyi
 **/

class Fan_speed_switch
{
  private:
    unsigned int fan_speed = 0;
    bool fan_speed_1 = false;
    bool fan_speed_2 = false;
  public:
    float temp_speed_1_start = 92;
    float temp_speed_1_stop  = 85;
    float temp_speed_2_start = 97;
    float temp_speed_2_stop  = 90;
    float temp = 0;
    float sensor_cut = -40;
    unsigned int fan_min_time = 30;  // minimal timer for turning on FAN, seconds

    int get_fan_speed()
    {
      // Manage abnormal circuit working
      if (temp <= sensor_cut) {
        Serial.println("Speed switch (E) Sensor is cut? Select 1st SPEED");
        fan_speed_1 = true;
        fan_speed = 1;
        return fan_speed;
      }

      // Manage 1st speed sensor
      if (temp >= temp_speed_1_start) {
        Serial.println("Speed switch (I) Select 1st SPEED");
        fan_speed_1 = true;
      }
      else if (temp > temp_speed_1_stop and temp < temp_speed_1_start and fan_speed_1 == true) {
        Serial.println("Speed switch (I) FAN is cooling with 1st SPEED");
        fan_speed_1 = true;
      }
      else {
        Serial.println("Speed switch (I) Stop 1st SPEED");
        fan_speed_1 = false;
      }

      // Manage 2nd speed sensor
      if (temp >= temp_speed_2_start) {
        Serial.println("Speed switch (I) Select 2nd SPEED");
        fan_speed_2 = true;
      }
      else if (temp > temp_speed_2_stop and temp < temp_speed_2_start and fan_speed_2 == true) {
        Serial.println("Speed switch (I) FAN is cooling with 1st SPEED");
        fan_speed_2 = true;
      }
      else {
        Serial.println("Speed switch (I) Stop 2nd SPEED");
        fan_speed_2 = false;
      }

      if (fan_speed_2) {
        fan_speed = 2;
      }
      else if (fan_speed_1) {
        fan_speed = 1;
      }
      else {
        fan_speed = 0;
      }

      return fan_speed;
    }
};
