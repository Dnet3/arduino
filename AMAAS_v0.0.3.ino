
/*
  DNET3 Group Project 2017:

  LAMM Security

  Group members: 

  Max Deasy
  Alan Kelly
  Leslie McCarthy
  Maciej Szaflarski
  
*/

#define CUSTOM_SETTINGS
#define INCLUDE_ACCELEROMETER_SENSOR_SHIELD
#define INCLUDE_GYROSCOPE_SENSOR_SHIELD
#define INCLUDE_CAMERA_SHIELD 
#define INCLUDE_EMAIL_SHIELD
#define INCLUDE_GPS_SHIELD
#define INCLUDE_SMS_SHIELD
#define INCLUDE_TWITTER_SHIELD
//include OneSheeld library
#include <OneSheeld.h>

boolean isAlertSent = false;
boolean active = true;
int arduinoID = 1; /* A ID for this Arduino. */
int redLED = 9;
int yellowLED = 8;
int greenLED = 7;
int onBoardLED = 13;
char emailAddr[40];
char telNo[15];
char lat[12];
char lon[12];

void setup() {
   /* Start communication.*/
  OneSheeld.begin();
  /* Set the red, yellow and green LEDs as outputs. */
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(onBoardLED, OUTPUT);

  //setting email address and phone number
  strcpy(emailAddr,"name.surname@mycit.ie");
  strcpy(telNo, "+353123456789");

  //set sms monitoring
  SMS.setOnSmsReceive(&smsReceived);

  //testing LEDs
  digitalWrite(redLED, HIGH);
  delay(500);
  digitalWrite(redLED, LOW);
  digitalWrite(yellowLED, HIGH);
  delay(500);
  digitalWrite(yellowLED, LOW);
  digitalWrite(greenLED, HIGH);
  delay(500);
  digitalWrite(greenLED, HIGH);  //on when arduino anti-theft monitoring active  
}

void loop() {

  //sendData();

  if(active){
    int x=AccelerometerSensor.getX();
    int y=AccelerometerSensor.getY();
    int z=AccelerometerSensor.getZ();
  
    if(GyroscopeSensor.getZ() > 1 && (sqrt((x*x) + (y*y) + (z*z)) > 13)){
        if(!isAlertSent){ 
          isAlertSent = sendAlert();
          delay(2000);
        }   
    }
    
    else{
      // Reset alert sent status
      isAlertSent = false;
      digitalWrite(redLED, LOW);
      digitalWrite(yellowLED, LOW);
    }
  }
}

void getCoordinates(){
  /* Use the existing OneSheeld code to get the latitude and longitude co-ordinates from the connected phone. */
  dtostrf(GPS.getLatitude(), 9, 7, lat); 
  dtostrf(GPS.getLongitude(), 9, 7, lon);
  delay(500); 
}

void takePicture(){
  //if(LightSensor.getValue()<100 && LightSensor.getValue()>0){
          /* Turn on the camera's flash while capturing. */
          //Camera.setFlash(ON);
          /* Take a photo using the phone's rear camera. */
          //Camera.setQuality(HIGH_QUALITY);
          //Camera.rearCapture();
          Camera.frontCapture();
          /* Delay for 5 seconds. */
          OneSheeld.delay(5000);
          //Camera.setFlash(OFF);
        //}
}

void smsReceived(char * phoneNumber, char * textMessage){
  //check phone number
  if(!strcmp(phoneNumber, telNo)){
    if(!strcmp(textMessage,"activate")){
      active = true;
      digitalWrite(greenLED, HIGH);
    } else if(!strcmp(textMessage,"deactivate")){
      active = false;
      digitalWrite(greenLED,LOW);
    } else if(!strcmp(textMessage,"report")){
      if(sendAlert()){
        digitalWrite(redLED, LOW);
        digitalWrite(yellowLED, LOW);
      }
    } else{
      char * token = strtok(textMessage, ":");
      if(token){
        if(!strcmp(token,"newEmail")){
          token = strtok(NULL, ":");
          if(token){
            strcpy(emailAddr,token);
            digitalWrite(redLED, HIGH);
            delay(600);
            digitalWrite(redLED, LOW);
          }
        }else if(!strcmp(token,"newPhone")){
          token = strtok(NULL, ":");
          if(token){
            strcpy(telNo, token);
            digitalWrite(yellowLED, HIGH);
            delay(600);
            digitalWrite(yellowLED, LOW);
          }
        }
      }
    }
  }
}

boolean sendAlert(){
    takePicture();
    getCoordinates();
    //String for direct google maps link
    char link[56] = "\nhttp://www.google.com/maps/place/";
    strcat(link,lat);
    strcat(link,",");
    strcat(link,lon);

    //Preparing email text
    char * email = (char*)malloc(130 * sizeof(char));
    strcpy(email,"Car position is\nLatitude: ");
    strcat(email,lat);
    strcat(email,"\nLongitude is: ");
    strcat(email,lon);      
    strcat(email,link);
    
    // Send the Email
    Email.attachLastPicture(emailAddr,"ALERT: Car position!",email, 0);
    //turn on redLED after sending email
    digitalWrite(redLED, HIGH);
    free(email);
    
    // Send the SMS
    char * text = (char*)malloc(120 * sizeof(char));
    strcpy(text,"ALERT: Car position:");
    strcat(text,link);
    SMS.send(telNo,text);
    //turn on yellowLED after sending text
    digitalWrite(yellowLED, HIGH);
    free(text);

    return true;
  
}

void sendData(){
    getCoordinates();
    /* A seperator to be used between data values. */
    char seperator[] = ";;;";
    /* Create a customized String that will contain data to be sent to the LAMM website. */
    String tweetString =  String(arduinoID);
    tweetString.concat(seperator);
    tweetString.concat(lat);
    tweetString.concat(seperator);
    tweetString.concat(lon);
    tweetString.concat(seperator);
    /* This last String will be used by the existing TwitterShield Android code to send a tweet. */
    tweetString.concat("REPLACE THIS TEXT WITH TWEET");
  
    /* Tweet last picture AND/OR run custom 1Sheeld Android code. */
    Twitter.tweetLastPicture(tweetString, 0);
}



