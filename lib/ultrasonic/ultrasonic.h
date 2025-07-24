#ifndef ULTRASONIC_H
#define ULTRASONIC_H
#include "Arduino.h"

class ULTRASONIC
{
  private:
  byte trig_pin=0;
  byte echo_pin=0;
  unsigned long duration = 0;
  float temp=0;
  float v=0.03492;
  unsigned long preves_time_messure=0;
  unsigned int timeInterval_messure=1000;
  float centimeter=0;

  public:
  ULTRASONIC(byte pingpin,byte inpin)
  {
    trig_pin = pingpin;
    echo_pin = inpin;
    timeInterval_messure=1000;
  }
  ULTRASONIC(byte pingpin,byte inpin,unsigned int time_task)
  {
    trig_pin = pingpin;
    echo_pin = inpin;
    timeInterval_messure=time_task;
    preves_time_messure=millis();
  }
  void begin()
  {
    pinMode(trig_pin, OUTPUT);
    pinMode(echo_pin, INPUT);
    preves_time_messure=millis();
  }
  // echo first and get time duration and convert to centimeter for millimeter 
  //echo ultrasonic to piont and wait sound to return to tranducer for messure time
  void echo()
  {
    digitalWrite(trig_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(trig_pin, HIGH);
    delayMicroseconds(5);
    digitalWrite(trig_pin, LOW);
    duration = pulseIn(echo_pin, HIGH);
    //return to_centimeter();
    //v=29;
    //return duration;
  }
 /* void echo(float temp_0)
  {
    Serial.println("1");
    digitalWrite(trig_pin, LOW);
    Serial.println("1");
    delayMicroseconds(2);
    Serial.println("1");
    digitalWrite(trig_pin, HIGH);
    Serial.println("1");
    delayMicroseconds(5);
    Serial.println("1");
    digitalWrite(trig_pin, LOW);
    Serial.println("1");
    duration = pulseIn(echo_pin, HIGH);
    Serial.println("1");
    //temp = ((temp_0-32)/1.8);
    //v=1.0/(((331.5+(0.61*temp))*100.0)/pow(10,6.0));
    //Serial.println(temp);
     //Serial.println(v);
    //return duration;
  }
*/
  //microsecond to centimeters
  void to_centimeter()
  {
    //Serial.print("Duration");
    //Serial.println(duration);
    if (duration == 0 || duration < 0)
      centimeter = 0;
    else
      centimeter=(duration * v )/ 2.0;
      //centimeter=duration / v / 2.0;
      //centimeter = duration / 29 / 2;
    //return centimeter;
  }
  float messure()
  {
    if ((millis() - preves_time_messure) > timeInterval_messure) {
      echo();
      to_centimeter();
      preves_time_messure = millis();
    }
    return centimeter;
  }
};
#endif
