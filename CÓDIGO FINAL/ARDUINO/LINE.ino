
char frontal_sensor[5];

void auto_tracking() {

  String sensorval = read_front_sensor_values();

  if (sensorval == "11000" || sensorval == "10000" ||
      sensorval ==
          "01000") // The black line is in the left of the car, need  left turn
  {

    go_Left();
    set_Motorspeed(FAST_SPEED, FAST_SPEED);
  } else if (sensorval == "10100" || sensorval == "00100" ||
             sensorval == "01100" || sensorval == "11100" ||
             sensorval == "10010" || sensorval == "11010") {
    go_Advance();
    set_Motorspeed(MID_SPEED, FAST_SPEED);

  } else if (sensorval == "00011" || sensorval == "00001" ||
             sensorval == "00010") { // The black line is  on the right of the
                                     // car, need  right turn

    go_Right();
    set_Motorspeed(FAST_SPEED, FAST_SPEED);
  } else if (sensorval == "00101" || sensorval == "00110" ||
             sensorval == "00111" || sensorval == "01101" ||
             sensorval == "01111" || sensorval == "01011" ||
             sensorval == "01110" || sensorval == "01001") {
    go_Advance();
    set_Motorspeed(FAST_SPEED, MID_SPEED);
  } else if (sensorval == "01110" || sensorval == "00100") {
    go_Advance();
    set_Motorspeed(SPEED, SPEED);
  } else if (sensorval == "11111") {
    stop_Stop();
    set_Motorspeed(0, 0);
  }

  else if (sensorval == "00000") {

    stop_Stop();
    go_Advance();
    set_Motorspeed(SPEED, SPEED);
  }
}

/* Read sensor value string, 1 stands for black, 0 starnds for white, i.e 10000
 * means the first sensor(from left) detect black line, other 4 sensors detected
 * white ground */
String read_front_sensor_values() {

  int sensorvalue = 32;

  frontal_sensor[0] = digitalRead(LFSensor_0);

  frontal_sensor[1] = digitalRead(LFSensor_1);

  frontal_sensor[2] = digitalRead(LFSensor_2);

  frontal_sensor[3] = digitalRead(LFSensor_3);

  frontal_sensor[4] = digitalRead(LFSensor_4);

  sensorvalue += frontal_sensor[0] * 16 + frontal_sensor[1] * 8 + frontal_sensor[2] * 4 +
                 frontal_sensor[3] * 2 + frontal_sensor[4];

  sensorvalue = ~sensorvalue;
  String senstr = String(sensorvalue, BIN);
  senstr = senstr.substring(11, 16);

  return senstr;
}


char sensor[5];

String read_sensor_values(){   
  
  int sensorvalue = 32; // Corresponde con 11111

  sensor[0]= !digitalRead(LFSensor_0);
 
  sensor[1]=!digitalRead(LFSensor_1);
 
  sensor[2]=!digitalRead(LFSensor_2);
 
  sensor[3]=!digitalRead(LFSensor_3);
 
  sensor[4]=!digitalRead(LFSensor_4);

  sensorvalue += sensor[0] * 16 + sensor[1] *8 + sensor[2]  *4 + sensor[3] * 2 + sensor[4];
  
  String senstr= String(sensorvalue,BIN);

  senstr=senstr.substring(1,6);

  return senstr;
  
}

