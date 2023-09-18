/**
 * Author:    Serhii Horelskyi
 * Created:   08.12.2019
 *
 * (c) Serhii Horelskyi
 **/

int analogPin = 0;
float raw = 0;     // Get temp voltage
float adc_max = 1024;
float R1 = 220;  // STD nominal
float R2 = 3520; // Default normal operate
float temp = 20.0;
float t_temp[3] = {20.0, 20.0, 20.0};
float dt_window = 5;
bool  debug = true;
//bool  debug = false;

// Temperatures for speeds
float temp_speed_1_start = 90;    // 
float temp_speed_1_stop  = 85;    //
float temp_speed_2_start = 97;    //
float temp_speed_2_stop  = 92;    //
int fan_speed = 0;
unsigned int  fan_min_time = 30;  // minimal timer for turning on FAN, seconds

float temp_cut = -40;
float temp_overheat = 100;

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

float temp_DB [23][2] = { // R ohm, Temp
                              177,   100,
                              241,    90,
                              332,    80,
                              467,    70,
                              667,    60,
                              973,    50,
                              1188,   45,
                              1459,   40,
                              1802,   35,
                              2238,   30,
                              2796,   25,
                              3520,   20,
                              4450,   15,
                              5670,   10,
                              7280,   5,
                              9420,   0,
                              12300,  -5,
                              16180,  -10,
                              21450,  -15,
                              28680,  -20,
                              52700,  -30,
                              100700, -40
                              };


float get_temp()
{
  int i = 0, n = 0;
  float delta_t = 0;
  float cache = 0, t_max = 0, t_min = 0;
  int size_t_temp = sizeof(t_temp)/sizeof(t_temp[0]);
  /*
   * Working ranges: 70-100 'C
   * ERRORS reanges: 
   *                > 100: R < 177 Ohm,  V < 1.36  - MAX speed
   *                < -40: R > 100K Ohm, V > 4.97  - 1st speed turned on always
   * t range:
   *  70-80:  delta1 = 13.5 Ohm/C
   *  80-90:  delta2 = 9.1 Ohm/C
   *  90-100:  delta3 = 6.4 Ohm/C
   *  t1 = 70, R = 467 Ohm
   *  t2 = 80, R = 332 Ohm
   *  t3 = 90, R = 241 Ohm
   *  t4 = 100,R = 177 Ohm
  */

  // fill temp massive
  for (n = 0; n < size_t_temp; n++){
    
      raw = analogRead(analogPin);   // Get value: 0-1023
      if(raw){
        if(debug){Serial.print("Raw: ");}
        if(debug){Serial.println(raw);}
      
        R2 = (R1 * raw)/ (adc_max - raw);
        if(debug){Serial.print("R2 : ");}
        if(debug){Serial.println(R2);}
      
        // Overheat detect
        if (R2 < temp_DB[0][0]){
          if(debug){Serial.print("OVERHEAT DETECTED, R:");}
          if(debug){Serial.println(R2);}
          if(debug){Serial.print("OVERHEAT DETECTED, TEMPERATURE over ");}
          if(debug){Serial.println(temp_DB[0][1]);}
          if(debug){Serial.println("Turn Fan speed at level: 2 (MAX)");}
          t_temp[n] = temp_overheat;
        }
        // Sensor is CUT
        else if (R2 > temp_DB[21][0]){          
          if(debug){Serial.print("Sensor is CUT, R:");}
          if(debug){Serial.println(R2);}
          if(debug){Serial.println("Turn FAN speed at level: 1 (LOW)");}
          t_temp[n] = temp_cut;
          //Serial.println(sizeof(temp_DB)/sizeof(temp_DB[0])-2);
          //Serial.println(temp_DB[21][0]);
        }
        else {
          for (i=0; R2 >= temp_DB[i][0]; i++){
            if(debug){Serial.println("Get tables");}
            if(debug){Serial.println(temp_DB[i][0]);}
            if(debug){Serial.println(temp_DB[i][1]);}
            if(debug){Serial.println(temp_DB[i+1][0]);}
            if(debug){Serial.println(temp_DB[i+1][1]);}
            delta_t = (temp_DB[i+1][0] - temp_DB[i][0]) / (temp_DB[i][1] - temp_DB[i+1][1]);
            if(debug){Serial.println(delta_t);}
            t_temp[n] = temp_DB[i+1][1] + (temp_DB[i+1][0] - R2)/delta_t;    
          }
        }
      }
      else {
          if(debug){Serial.print("Can't get TEMP data, R:");}
          if(debug){Serial.println(R2);}
          if(debug){Serial.println("Turn FAN speed at level: 1 (LOW)");}
          t_temp[n] = temp_cut;
      }
    
  if(debug){
    Serial.println("*****t_temp*****");
    Serial.println(n+1);
    Serial.println(t_temp[0]);
    Serial.println(t_temp[1]);
    Serial.println(t_temp[2]);
    Serial.println("*****t_temp*****");
    }
  delay(250);
  }

  // validate temp by t_window
  t_min = t_temp[0];
  t_max = t_temp[0];
  cache = t_temp[0];
  for (n = 1; n < size_t_temp; n++){
    if (t_temp[n] > t_max) {t_max = t_temp[n];}
    if (t_temp[n] < t_min) {t_min = t_temp[n];}
    cache = cache + t_temp[n];
    if (n == size_t_temp-1) {cache = cache / (size_t_temp);}
  }
  if (t_max - t_min > dt_window){
    if(debug){Serial.println("Temperature Validation");}
    if(debug){Serial.println(t_min);}
    if(debug){Serial.println(t_max);}
    if(debug){Serial.println("BAD Temperature data, use previous TEMP values");}
    return temp;
  }
  else{
    temp = cache;
    return temp;
  }
  return t_temp[0];
}

int get_fan_speed()
{
    // START FAN SPEED
    // T <= -40 : start if temp is abnormal, temp <= '-40'
    if (temp <= temp_cut){
      Serial.println("Start FAN at 1st speed, abnormal working. Please, check FAN circuit!"); 
      fan_speed = 1;
    }
    // Engine heating
    // -40 < T <= 85, Engine is cooled
    if (temp_cut < temp and temp <= temp_speed_1_stop){
      Serial.println("FAN stopped, Engine is cooled");
      fan_speed = 0;
    }
    // 85 < T < 90, Engine is warming
    if (temp_speed_1_stop < temp and temp < temp_speed_1_start and fan_speed == 0){
      Serial.println("FAN stopped, Engine is not heated");
      fan_speed = 0;
    }
    // 90 <= T < 92, Engine is warm
    if (temp_speed_1_start <= temp and temp < temp_speed_2_stop){
      Serial.println("FAN 1st speed enable, Engine is warm");
      fan_speed = 1;
    }
    // 92 <= T < 97, Engine is almost HOT
    if (temp_speed_2_stop <= temp and temp < temp_speed_2_start and fan_speed != 2){
      Serial.println("FAN 1st speed enable, Engine is almost HOT");
      fan_speed = 1;
    }
    // 97 <= T, Engine is HOT
    if (temp_speed_2_start <= temp or temp >= temp_overheat){
      Serial.println("FAN 2nd speed enable, Engine is HOT");
      fan_speed = 2;
    }
    // Engine cooling
    // 92 <= T < 97, Engine is almost HOT and cooling
    if (temp_speed_2_stop <= temp and temp < temp_speed_2_start and fan_speed == 2){
      Serial.println("FAN 1st speed enable, Engine is almost HOT");
      fan_speed = 2;
    }  
    /*// 90 <= T < 92, Engine is still warm
    if (temp_speed_1_start <= temp and temp < temp_speed_2_stop){
      Serial.println("FAN 1st speed enabled, Engine is warm");
      fan_speed = 1;
    }*/   
    // 85 < T < 90, Engine is almost cooled
    if (temp_speed_1_stop < temp and temp < temp_speed_1_start and fan_speed == 1){
      Serial.println("FAN 1st speed enable, Engine is warm");
      fan_speed = 1;
    }   
    return fan_speed;
}

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  
    temp = get_temp();
    Serial.print("Current Temperatue: ");
    Serial.println(temp);    
    
    fan_speed = get_fan_speed();
    Serial.print("Select FAN speed: ");
    Serial.println(fan_speed);    
  
    delay(1000);
}















