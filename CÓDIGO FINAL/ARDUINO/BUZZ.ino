
void buzz_ON()   //open buzzer
{  
  digitalWrite(PIN_BUZZ,LOW);
  delay(50);//wait for 10ms
  digitalWrite(PIN_BUZZ,HIGH);  
}

void buzz_OFF()  //close buzzer
{
  digitalWrite(PIN_BUZZ, HIGH);  
}

void alarm(){
   buzz_ON(); 
   buzz_OFF();
}