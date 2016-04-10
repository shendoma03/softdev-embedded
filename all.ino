float temp;
int tempPin = A0;
int sprinkle = 6;
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

void loop() {


  int wtrLevel = analogRead(A1);//
    int smsAnalog = analogRead(A3);//
    int smsDigital = digitalRead(7);//
    
    temp = analogRead(tempPin);

    temp = temp * 0.48828125;
    
     pinMode(sprinkle, OUTPUT); //
   
 
 
  Serial.print("Water Level =  "); Serial.print(wtrLevel);Serial.println("   ");
  delay(3000);//delay ms.
//  Serial.print("Soil moisture =  "); Serial.print(smsAnalog);
//   delay(3000);//delay ms.
//  Serial.print(" meron ba =  "); Serial.print(smsDigital);Serial.println("   ");
//  delay(3000);//delay ms.
//Serial.print("TEMPRATURE = ");Serial.print(temp);Serial.print("*C");Serial.println("   ");
// delay(5000);//delay ms.
// if ( smsAnalog >600 && smsDigital == 0 ) {
//    digitalWrite(sprinkle, HIGH); 
// 
// }else {
// digitalWrite(sprinkle, LOW); 
 }


 
 
 


