void task1()
{
  Serial.println("begin task1");
  unsigned long m = millis();
  int temp = 0;
  
  for(int i=0; i < 30000; i++)
    {temp++; temp = temp%100; }
  for(int i=0; i < 30000; i++)
    {temp++; temp = temp%100; }
  for(int i=0; i < 11000; i++)
    { temp++; temp = temp%100; }


   
    
  Serial.print(temp);
  Serial.print(" end of task1: ");
  Serial.println(millis());
}





void setup() {
  Serial.begin(19200);
    task1();
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10000);

}
