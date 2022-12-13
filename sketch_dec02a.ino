#include <LiquidCrystal.h>
#include <Wire.h>

#define Motor_forward         0
#define Motor_return          1
#define Motor_L_dir_pin       7
#define Motor_R_dir_pin       8
#define Motor_L_pwm_pin       9
#define Motor_R_pwm_pin       10

#define CMPS14_address 0x60

const int rs = 52, en = 50, d4 = 42, d5 = 40, d6 = 38, d7 = 36;
double radius = 3;
double p = 18.84;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
double distance1 = 0, distance2 = 0;
int count1 = 0;
int count2 = 0;
int xRS=0, yRS=0;
double pulseToCm = 0.068;
#define ENC_A_L 3
#define ENC_B_L 2
#define ENC_A_R 23
#define ENC_B_R 24
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(24, 4);
  Wire.begin();
  attachInterrupt(digitalPinToInterrupt(ENC_B_L), isr1, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_A_L), isr2, RISING);
}

void loop() {
  // Get the string message from ESP inside Serial Monitor
    String message = Serial.readStringUntil('\n'); 
     
    Serial.println(message);
    int pos_s = message.indexOf("Print");
    String stat = message.substring(pos_s + 1);
    lcd.setCursor(0,0);
    lcd.print(stat);
    // Convert the value of distance/degrees for the robot to move
    int d = stat.toInt();
    // If the signal does not contain the Turn keyword => move the robot upwards or forwards from the value of distance in Serial monitor
    if (d > 0 && stat.indexOf("Turn") <= 0 ){
      while(distance1 < d){
        digitalWrite(Motor_R_dir_pin,1);  
        digitalWrite(Motor_L_dir_pin,1);
        analogWrite(Motor_L_pwm_pin,254);
        analogWrite(Motor_R_pwm_pin,254);
        distance1 = pulseToCm*count1;
        
      }
      
    }
    else if(d < 0 && stat.indexOf("Turn") <= 0 ){
      while(distance1 < abs(d)){
        digitalWrite(Motor_R_dir_pin,0);  
        digitalWrite(Motor_L_dir_pin,0);
        analogWrite(Motor_L_pwm_pin,254);
        analogWrite(Motor_R_pwm_pin,254);
        distance1 = pulseToCm*count2;
        
      }
      
    }
   // If the signal contains the Turn keyword => Turn the robot from the value of degree in Serial monitor

    else if(d > 0 && stat.indexOf("Turn") > 0 ){
      
      while(pulseToCm*count2<abs(d*0.11)){
        digitalWrite(Motor_R_dir_pin,1);  
        digitalWrite(Motor_L_dir_pin,1);
        analogWrite(Motor_L_pwm_pin,0);
        analogWrite(Motor_R_pwm_pin,254);
        distance1 = pulseToCm*count2;
        
       
      }
    }
    else {
      
      while(pulseToCm*count1<abs((d*0.11))){
        digitalWrite(Motor_R_dir_pin,1);  
        digitalWrite(Motor_L_dir_pin,1);
        analogWrite(Motor_L_pwm_pin,254);
        analogWrite(Motor_R_pwm_pin,0);
        distance1 = pulseToCm*count1;
        
      }
    }
    analogWrite(Motor_L_pwm_pin,0);
    analogWrite(Motor_R_pwm_pin,0);
    distance1 = 0;
    count1 = 0;
    count2 = 0;

//  Read the value of x&y joystick from analog Reader
    int xValue = analogRead(A0);
    int yValue = analogRead(A15); 
//  Map x value to speed and y value to direction
    int xP = map(xValue, 0, 1023, 0, 100);
    int yP = map(yValue, 0, 1023, -100, 100);
//  Put the robot at rest when joystick is not moving
      if(yP<10 && yP>-10 && xP<60 && xP>40 && d==0 || yP<10 && yP>-10 && xP<60 && xP>40){
        analogWrite(Motor_L_pwm_pin,0);
        analogWrite(Motor_R_pwm_pin,0); 
      }
// Control the robot downwards
      else if(yP < 0){
        digitalWrite(Motor_R_dir_pin,0);  
        digitalWrite(Motor_L_dir_pin,0);
        analogWrite(Motor_L_pwm_pin,xP*2);
        analogWrite(Motor_R_pwm_pin,(100-xP)*2); 
// Control the robot upwards
      } else  {
        digitalWrite(Motor_R_dir_pin,1);  
        digitalWrite(Motor_L_dir_pin,1);
        analogWrite(Motor_L_pwm_pin,xP*2);
        analogWrite(Motor_R_pwm_pin,(100-xP)*2); 

      }

    
    distance1 = 0;
    count1 = 0;
    count2 = 0;
      
    // Check the compass
  char *myStrings[]={"N","NE","E","SE","S","SW","W","NW"};
  
  Wire.beginTransmission(CMPS14_address);
  Wire.write(1);
  Wire.endTransmission(false);
//If the compass is available
  Wire.requestFrom(CMPS14_address, 1, true);
  if(Wire.available() >= 1){
     byte raw = Wire.read();
     int rawData = map(raw, 0, 255, 0, 360);
     lcd.setCursor(0,0);
     //Print the degree
     lcd.print("                      ");
     lcd.setCursor(0,0);
     lcd.print(rawData);
     
     lcd.setCursor(1,1);
     lcd.print("                      ");
     int direction = map(raw, 0, 255, 0, 7);
     lcd.setCursor(1,1);
     //Print the direction of North, South, East West
     lcd.print(myStrings[direction]);
     
     
     
  }
   
     
       
    
}

void isr1(){
  count1++;
}
void isr2(){
  count2++;
}
