/* WIFI SETTINGS */
const char* ssid = "MAGAN";
const char* pass = "iJEwLmvZLANw7Z47EgCx";

void setupWifi(){

  Serial1.begin(115200);  
  Serial1.write("AT+CIOBAUD=9600\r\n");
  Serial1.write("AT+RST\r\n");
  Serial1.begin(9600);
  WiFi.init(&Serial1);
  // Check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial1.println("WiFi shield not present");    
    while (true);
  }
  // Attempt to connect to WiFi network
  while ( wifi_status != WL_CONNECTED) {  
    wifi_status = WiFi.begin(ssid, pass);
  }  
  printWifiStatus();
  delay(1500);
}

void printWifiStatus()
{
  // Print the SSID of the network
  Serial.print("\tSSID: ");
  Serial.println(WiFi.SSID());

  // Print IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("\tIP Address: ");
  Serial.println(ip);

  // Print MAC address
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("\tMAC Address: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX); 

}