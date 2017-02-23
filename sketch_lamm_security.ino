/*
  DNET3 Group Project 2017:

  LAMM Security

  Group members: 

  Max Deasy
  Alan Kelly
  Leslie McCarthy
  Maciej Szaflarski
  
*/

/* Include OneSheelds to include. */
#define CUSTOM_SETTINGS
#define INCLUDE_ACCELEROMETER_SENSOR_SHIELD
#define INCLUDE_GYROSCOPE_SENSOR_SHIELD
#define INCLUDE_GPS_SHIELD
#define INCLUDE_TWITTER_SHIELD


#include <OneSheeld.h> /* Include OneSheeld library. */


int arduinoID = 0; /* A ID for this Arduino. */

/* A name for the LEDs and the pins they correspond with. */
int redLED = 9;
int yellowLED = 8;
int greenLED = 7;
int onBoardLED = 13;

void setup() {
  
  /* Start communication. */
  OneSheeld.begin();
 
  /* Set the red, yellow and green LEDs as outputs. */
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(onBoardLED, OUTPUT);
}

void loop() {
  
  /* Check Z-axis motion value.  */
  if (GyroscopeSensor.getZ() > 1 && (AccelerometerSensor.getY() > 4 || AccelerometerSensor.getX() > 4)) {
    
    /* Flash the green LED on the Arduino 5 times to signify that movement parameters have been met. */
    for (int i = 0; i < 5; i++) {
      digitalWrite(greenLED, HIGH);
      digitalWrite(onBoardLED, HIGH);
      OneSheeld.delay(250);
      digitalWrite(greenLED, LOW);
      digitalWrite(onBoardLED, LOW);
      OneSheeld.delay(250);
    }

    /* Turn on the red LED. This is to simulate turning off the engine. */
    digitalWrite(redLED, HIGH);
    
    /* Turn on the camera flash. */
    Camera.setFlash(ON);
    /* Take a picture. */
    Camera.rearCapture();

   /* IMPORTANT: Use OneSheeld delay for better performance. */
    OneSheeld.delay(5000);
    /* Turn on the yellow LED. This is to signify that the Camera code has completed. */
    digitalWrite(yellowLED, HIGH);

    char latBuff[12];
    char longBuff[12];
    char seperator[] = ";;;";
    /* Create a customized String that will contain data to be sent to the LAMM website. */
    String tweetString =  String(arduinoID);
    /* Use a seperator between data values. */
    tweetString.concat(seperator);
    /* Use the existing OneSheeld code to get the latitude co-ordinates from the connected phone. */
    tweetString.concat(dtostrf(GPS.getLatitude(), 9, 7, latBuff));
    /* IMPORTANT: Use OneSheeld delay for better performance. */
    OneSheeld.delay(500);
    tweetString.concat(seperator);
    /* Use the existing OneSheeld code to get the longitude co-ordinates from the connected phone. */
    tweetString.concat(dtostrf(GPS.getLongitude(), 9, 7, longBuff));
    /* IMPORTANT: Use OneSheeld delay for better performance. */
    OneSheeld.delay(500);
    tweetString.concat(seperator);
    /* This part of the String will be used by the existing TwitterShield Android code to send a tweet. */
    tweetString.concat("REPLACE THIS TEXT WITH TWEET");

    /* Tweet last picture AND/OR run custom 1Sheeld Android code. */
    Twitter.tweetLastPicture(tweetString, 0);
    /* Turn on the green LED. This is to signify Arduino code completion. */
    digitalWrite(greenLED, HIGH);
  }

  /* Turn off the LEDs.*/
  digitalWrite(redLED, LOW);
  digitalWrite(yellowLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(onBoardLED, LOW);
}
