/**
 * Author:    Serhii Horelskyi
 * Created:   06.06.2020
 *
 * (c) Serhii Horelskyi
 **/

class Temperature_sensor {
  private:
    unsigned int analogPin = 0;         // pin A0
    unsigned int ADC_Raw = 0;           // Get temp voltage
    unsigned int ADC_MAX = 1023;
    float t_temp[3] = {0, 0, 0};
    float temp = 0;
  public:
    unsigned int R1 = 220;              // STD nominal
    unsigned int R_Sensor = 3520;       // Default normal operate
    unsigned int dt_window = 5;
    unsigned int error = 0;             // 0 - normal operate, 1 - low impendance(overheat), 2 - hi empendance (cut), 3 - cant get data (short circuit)

    /*
      T   R1  R2    Vout
      100 470 177   1,36785162287481  OVERHEAT
      90  470 241   1,69479606188467
      80  470 332   2,06982543640898
      70  470 467   2,49199573105656
      60  470 667   2,93315743183817
      50  470 973   3,37144837144837
      45  470 1188  3,58262967430639
      40  470 1459  3,7817522032141
      35  470 1802  3,96566901408451
      30  470 2238  4,13220088626292
      25  470 2796  4,28046540110227
      20  470 3520  4,41102756892231
      15  470 4450  4,52235772357724
      10  470 5670  4,61726384364821
      5   470 7280  4,69677419354839
      0   470 9420  4,7623862487361
      -5  470 12300 4,8159749412686
      -10 470 16180 4,85885885885886
      -15 470 21450 4,89279197080292
      -20 470 28680 4,91938250428817
      -30 470 52700 4,9558021440662
      -40 470 100700  4,97677177028764
      CUT 470 200000  4,98827754776276  FAIL TEMP SENSOR
    */

    // R ohm, Temp
    int temp_DB [25][2] = {
      {65,    130},
      {91,    120},
      {127,   110},
      {177,   100},
      {241,    90},
      {332,    80},
      {467,    70},
      {667,    60},
      {973,    50},
      {1188,   45},
      {1459,   40},
      {1802,   35},
      {2238,   30},
      {2796,   25},
      {3520,   20},
      {4450,   15},
      {5670,   10},
      {7280,   5},
      {9420,   0},
      {12300,  -5},
      {16180,  -10},
      {21450,  -15},
      {28680,  -20},
      {52700,  -30},
      {100700, -40}
    };


    float get_temp()
    {
      unsigned int i = 0, n = 0;
      float delta_t = 0;
      float cache = 0, t_max = 0, t_min = 0;
      unsigned int size_t_temp = sizeof(t_temp) / sizeof(t_temp[0]);
      unsigned int size_temp_DB = sizeof(temp_DB) / sizeof(temp_DB[0]);
      // fill temp massive
      for (n = 0; n < size_t_temp; n++) {
        ADC_Raw = analogRead(analogPin);
        Serial.print("Sensor (D) - ADC Raw: ");
        Serial.println(ADC_Raw);
        if (ADC_Raw) {
          if (ADC_Raw == ADC_MAX) {
            t_temp[n] = temp_DB[size_temp_DB - 1][1];
            error = 2;
            Serial.println("Sensor (E) - Sensor is CUT?");
          }
          else if (ADC_Raw == 0) {
            t_temp[n] = temp_DB[0][1];
            error = 1;
            Serial.println("Sensor (E) - Sensor is SHORTCUT?");
          }
          else {
            R_Sensor = int(float(R1) * float(ADC_Raw) / (ADC_MAX - ADC_Raw));
            Serial.print("Sensor (E) - R Sensor, Ohm: ");
            Serial.println(R_Sensor);

            // Overheat detect
            if (R_Sensor < temp_DB[0][0]) {
              t_temp[n] = temp_DB[0][1];
              error = 1;
              Serial.println("Sensor (E) - Overheat detect");
            }
            // Sensor is CUT
            else if (R_Sensor > temp_DB[size_temp_DB - 1][0]) {
              t_temp[n] = temp_DB[size_temp_DB - 1][1];
              error = 2;
              Serial.println("Sensor (E) - Sensor is CUT");
              // Calc current temperature
            }
            else {
              for (i = 0; R_Sensor >= temp_DB[i][0]; i++) {
                delta_t = (temp_DB[i + 1][0] - temp_DB[i][0]) / (temp_DB[i][1] - temp_DB[i + 1][1]);
                t_temp[n] = temp_DB[i + 1][1] + (temp_DB[i + 1][0] - R_Sensor) / delta_t;
              }
              error = 0;
            }
          }
        }

        // Can't get TEMP, set FAKE temp for OVERHEAT
        else {
          t_temp[n] = temp_DB[0][1];
          error = 3;
          Serial.println("Sensor (E) - Can't get TEMP");
        }
        delay(333);
      }

      // validate temp by t_window
      Serial.println("Sensor (D) =====> t_temp <=====");
      Serial.print("Sensor (D) - test temp #1, 'C: ");
      Serial.println(t_temp[0]);
      Serial.print("Sensor (D) - test temp #2, 'C: ");
      Serial.println(t_temp[1]);
      Serial.print("Sensor (D) - test temp #3, 'C: ");
      Serial.println(t_temp[2]);
      Serial.println("Sensor (D) =====> t_temp <=====");
      t_min = t_temp[0];
      t_max = t_temp[0];
      cache = t_temp[0];
      for (n = 1; n < size_t_temp; n++) {
        if (t_temp[n] > t_max) {
          t_max = t_temp[n];
        }
        if (t_temp[n] < t_min) {
          t_min = t_temp[n];
        }
        cache = cache + t_temp[n];
        if (n == size_t_temp - 1) {
          cache = cache / (size_t_temp);
        }
      }
      if (t_max - t_min <= dt_window) {
        // Update temp if data is valid. Else - use previous data
        temp = cache;
      }
      Serial.print("Sensor (I) - Temperature, 'C: ");
      Serial.println(temp);
      return temp;
    }
};
