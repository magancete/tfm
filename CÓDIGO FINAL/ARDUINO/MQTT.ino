
//const char* mqtt_server = "magan.now-dns.org";
const char* mqtt_server = "192.168.1.25";

void callback(char* topic, byte* payload, unsigned int length) {

  alarm();

  Serial.print("[MQTT] Message arrived | ");
  Serial.print(topic);
  Serial.print(" | ");

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println(); 

  char c = payload[0]; 

  if (strcmp(topic,"mode")==0){
    switch (c){ 
      case 'M':
      changeMode(MANUAL_DRIVE);
      break;
      case 'O':
      changeMode(AUTO_DRIVE_UO);
      break;
      case 'T':
      changeMode(AUTO_DRIVE_LF);
      break;
      default:      
      break;
    }
  }
 
  if (strcmp(topic,"drive")==0) {
    
    if(Drive_Status==MANUAL_DRIVE){
    switch (c){
      case 'A': 
      WorkMode="GO_ADVANCE";
      break;
      case 'L':
      WorkMode="GO_LEFT";
      break;
      case 'R':
      WorkMode="GO_RIGHT";
      break;
      case 'B':
      WorkMode="GO_BACK";
      break;
      case 'S':
      WorkMode="STOP_STOP";
      break;      
      case 'G':
      SPEED=SPEED+10;
      if(SPEED>200){
        SPEED=200;
      }
      publisToTopic("speed", String(SPEED));  
      break;
      case 'J':
      SPEED=SPEED-10;
      if(SPEED<80){
        SPEED=80;
      }
      publisToTopic("speed", String(SPEED));  
      break;
      default:
      break;
    }
  }
  } 

}

void setupMqtt(){

  mqtt_client.setServer(mqtt_server, 1883);
  mqtt_client.setKeepAlive(90);
  mqtt_client.setSocketTimeout(60);
  mqtt_client.setBufferSize(512);
  mqtt_client.setCallback(callback); 

}

bool reconnect(){  

  Serial.println("[MQTT] Attempting MQTT connection..."); 
  
  clientId += String(random(0xffff), HEX);

  if (mqtt_client.connect(clientId.c_str())) {
    Serial.println("[MQTT] Connected");
    subscribeTopic("mode");
    subscribeTopic("drive");
    publisToTopic("speed", String(SPEED));  
  }else {
      Serial.print("[MQTT] Failed, rc=");
      Serial.println(mqtt_client.state());      
  }
  return mqtt_client.connected();

}

void subscribeTopic(char* topic){

  int qos = 1;

  if(mqtt_client.subscribe(topic,qos)){
    Serial.print("[MQTT] Subscribed to topic -> ");
    Serial.println(topic);
  }else{
    Serial.print("[MQTT] Error subscibing to topic -> ");
    Serial.println(topic);
  }  

}

void publisToTopic(char* topic, String msg){

  int str_len = msg.length() + 1; 
  char char_array[str_len];
  msg.toCharArray(char_array, str_len);

  if(mqtt_client.publish(topic,char_array)){
    Serial.print("[MQTT] Msg sended to topic ");
    Serial.print(topic);
    Serial.print(" -> ");
    Serial.println(char_array);
  }else{
    Serial.print("[MQTT] Error sending msg");
  } 

}

void send_sensor_values(){  

  long now = millis();

  if (now - lastMsg > 5000) {

    lastMsg = now;

    get_gps_position();

    get_gyro();

    //publisToTopic("frontal-sensor", read_front_sensor_values());      

    //publisToTopic("gps", gpsPosition);
    /*
    if(get_gps_position()){
      //publisToTopic("gps", gpsPosition);
    }  else{
      Serial.println("GPS no sincronizado");
    }*/

  }  

}
