int leftscanval; 
int centerscanval; 
int rightscanval;
int ldiagonalscanval; 
int rdiagonalscanval;

int numcycles = 0;

const int distancelimit = 30; // Distancia límite de parada
const int sidedistancelimit = 30; // Distancia límite de parada

int thereis=0;

const int loop_counter = 2; // Cuantas veces deber analizar el servo

const int turntime = 250; // Time the robot spends turning (miliseconds)
const int backtime = 300; // Time the robot spends turning (miliseconds)

int calc_distancia(){

  long echo_distance;
  digitalWrite(PIN_ULTRASONIC_TRIG,LOW);
  delayMicroseconds(5);                                                                              
  digitalWrite(PIN_ULTRASONIC_TRIG,HIGH);
  delayMicroseconds(15);
  digitalWrite(PIN_ULTRASONIC_TRIG,LOW);

  echo_distance=pulseIn(PIN_ULTRASONIC_ECHO,HIGH);

  echo_distance=echo_distance*0.01657; // Devuelve la distancia en cm  
  
  return round(echo_distance);

}

//Meassures distances to the right, left, front, left diagonal, right diagonal and asign them in cm to the variables rightscanval, 
//leftscanval, centerscanval, ldiagonalscanval and rdiagonalscanval (there are 5 points for distance testing)
String watchsurrounding(){
/*  obstacle_status is a binary integer, its last 5 digits stands for if there is any obstacles in 5 directions,
 *   for example B101000 last 5 digits is 01000, which stands for Left front has obstacle, B100111 means front, right front and right ha
 */
 
int obstacle_status = B100000;

  centerscanval = calc_distancia();

  if(centerscanval < distancelimit){
    stop_Stop();
    alarm();
    obstacle_status  = obstacle_status | B100;
  }

  head.write(120);
  delay(100);

  ldiagonalscanval = calc_distancia();

  if(ldiagonalscanval<distancelimit){
    stop_Stop();
    alarm();
    obstacle_status = obstacle_status | B1000;
  }

  head.write(170);
  delay(300);

  leftscanval = calc_distancia();

  if(leftscanval<sidedistancelimit){
    stop_Stop();
    alarm();
    obstacle_status  =obstacle_status | B10000;
  }

  head.write(90); 
  delay(100);

  centerscanval = calc_distancia();

  if(centerscanval<distancelimit){
    stop_Stop();
    alarm();
    obstacle_status = obstacle_status | B100;
  }

  head.write(40);
  delay(100);

  rdiagonalscanval = calc_distancia();

  if(rdiagonalscanval<distancelimit){
    stop_Stop();
    alarm();
    obstacle_status = obstacle_status | B10;
  }

  head.write(0);
  delay(100);

  rightscanval = calc_distancia();

  if(rightscanval<sidedistancelimit){
    stop_Stop();
    alarm();
    obstacle_status = obstacle_status | 1;  
  }

  head.write(90);
  delay(300);

  String obstacle_str = String(obstacle_status,BIN);
  obstacle_str= obstacle_str.substring(1,6);
  
  return obstacle_str;

}

void auto_avoidance(){

  ++numcycles;

  if(numcycles >= loop_counter){

    stop_Stop();
    String obstacle_sign = watchsurrounding(); // 5 digits of obstacle_sign binary value means the 5 direction obstacle status
      
    if( obstacle_sign=="10000"){   
     set_Motorspeed(FAST_SPEED,SPEED);
     go_Advance(); 
     delay(turntime);
     stop_Stop();
    }else if( obstacle_sign=="00001"  ){     
      set_Motorspeed(SPEED,FAST_SPEED);
      go_Advance();  
      delay(turntime);
      stop_Stop();
    }
    else if( obstacle_sign=="11100" || obstacle_sign=="01000" || obstacle_sign=="11000"  || obstacle_sign=="10100"  || obstacle_sign=="01100" ||obstacle_sign=="00100"  ||obstacle_sign=="01000" ){     
	    go_Right();
      set_Motorspeed(TURN_SPEED,TURN_SPEED);
      delay(turntime);
      stop_Stop();
    } 
    else if( obstacle_sign=="00010" || obstacle_sign=="00111" || obstacle_sign=="00011"  || obstacle_sign=="00101" || obstacle_sign=="00110" || obstacle_sign=="01010" ){
      go_Left();//Turn left
      set_Motorspeed(TURN_SPEED,TURN_SPEED);
      delay(turntime);
      stop_Stop();
    }else if(  obstacle_sign=="01111" ||  obstacle_sign=="10111" || obstacle_sign=="11111"  ){
   	  go_Left();
		  set_Motorspeed( FAST_SPEED,SPEED);
      delay(backtime);
      stop_Stop();
    }else if( obstacle_sign=="11011"  ||    obstacle_sign=="11101"  ||  obstacle_sign=="11110"  || obstacle_sign=="01110"  ){
      go_Right();
      set_Motorspeed( SPEED,FAST_SPEED);
      delay(backtime);
      stop_Stop();
    }else{
      numcycles = 0;
    }
  } else {
     set_Motorspeed(SPEED,SPEED);
     go_Advance();
     delay(backtime);
     stop_Stop();
  }  
  
  int distance = calc_distancia();
  if (distance<distancelimit){ 
    go_Right();
    set_Motorspeed( SPEED,FAST_SPEED);
    delay(backtime*3/2);
      ++thereis;
  }
  if (distance>distancelimit){
      thereis=0;
  }
  if (thereis > 25){
    stop_Stop();
    thereis=0;
  }
  
}