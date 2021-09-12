void get_gps_position() {

  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;   
   
   for (unsigned long start = millis(); millis() - start < 1000;)
   {
      while (Serial2.available())
      {
         char c = Serial2.read();
         if (gps.encode(c)) // Nueva secuencia recibida
            newData = true;
      }
   }

   if (newData)
   {
      float flat, flon;
      unsigned long age;
      gps.f_get_position(&flat, &flon, &age);

      short sats = gps.satellites();
      long hdop = gps.hdop();
      
      gpsPosition = clientId + "|" + String(flat) + "|" + String(flon) + "|" + String(sats) + "|" + String(hdop);
      publisToTopic("gps", gpsPosition);
      
   }     
   
   /*
   gps.stats(&chars, &sentences, &failed);
   Serial.print(" CHARS=");
   Serial.print(chars);
   Serial.print(" SENTENCES=");
   Serial.print(sentences);
   Serial.print(" CSUM ERR=");
   Serial.println(failed); 
  */

}
