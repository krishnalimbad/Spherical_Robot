
//AUTHOR: KRISHNA LIMBAD
//TOPIC: SPHERICAL ROBOT
//**************************************************
#include<SoftwareSerial.h>
SoftwareSerial gsm (2, 3); //make RX arduino line is pin 2, make TX arduino line is pin 3.
SoftwareSerial gps(4, 5);  //make RX arduino line is pin 4, make TX arduino line is pin 5.

#include <dht.h>

dht DHT;

#define DHT11_PIN 13

int monitoring;
int metalDetection = 1;

int state;

const int motorA1  = 8;  // Pin  2 of L293
const int motorA2  = 9;  // Pin  7 of L293

const int motorB1  = 10; // Pin 10 of L293
const int motorB2  = 11;  // Pin 14 of L293

int i = 0, k = 0;
int  gps_status = 0;
float latitude = 0;
float logitude = 0;
String Speed = "";
String gpsString = "";
char *test = "$GPRMC";

void initModule(String cmd, char *res, int t)
{
  while (1)
  {
    Serial.println(cmd);
    gsm.println(cmd);
    delay(100);
    while (gsm.available() > 0)
    {
      if (gsm.find(res))
      {
        Serial.println(res);
        delay(t);
        return;
      }

      else
      {
        Serial.println("Error");
      }
    }
    delay(t);
  }
}



void setup()
{

  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);

  
  gsm.begin(9600);//GSM
  delay(500);
  Serial.begin(9600);//BLUETOOTH

delay(500);
  Serial.println("Initializing....");
  initModule("AT", "OK", 1000);
  initModule("ATE1", "OK", 1000);
  initModule("AT+CPIN?", "READY", 1000);
  initModule("AT+CMGF=1", "OK", 1000);
  initModule("AT+CNMI=2,2,0,0,0", "OK", 1000);
  Serial.println("Initialized Successfully");

  delay(2000);
  gps.begin(9600);//GPS
  delay(2000);
  Serial.println("System Ready..");
  //get_gps();
  //show_coordinate();
}

void loop()
{


  if (Serial.available() > 0) {
    state = Serial.read();
  }

/***********************Tempreture****************************/
    if (state == 'T') {
      
     int chk = DHT.read11(DHT11_PIN);
     Serial.print("Temperature = ");
     Serial.println(DHT.temperature);
     Serial.print("Humidity = ");
     Serial.println(DHT.humidity);
     delay(1000);
      
    }


 /***********************metal detector****************************/
     if (state == 'M') {
      
     monitoring = analogRead(metalDetection);

    if (monitoring > 150)
    Serial.println(" No Metal Detected");
 
    if (monitoring < 150)
    Serial.println("Metal is Detected");

  }




  /***********************Forward****************************/
  //If state is equal with letter 'F', car will go forward!
  if (state == 'F') {
    digitalWrite(motorA1, HIGH);
    digitalWrite(motorA2, LOW);
    digitalWrite(motorB1, LOW);
    digitalWrite(motorB2, LOW);
    Serial.println("Forword");
  }

  /***********************Backward****************************/
  //If state is equal with letter 'B', car will go backward
  else if (state == 'B') {
    digitalWrite(motorA1, LOW);
    digitalWrite(motorA2, HIGH);
    digitalWrite(motorB1, LOW);
    digitalWrite(motorB2, LOW);
    Serial.println("Backword");
  }

  /***************************Left*****************************/
  //If state is equal with letter 'L', wheels will turn left
  else if (state == 'L') {
    digitalWrite(motorA1, LOW);
    digitalWrite(motorA2, LOW);
    digitalWrite(motorB1, HIGH);
    digitalWrite(motorB2, LOW);
    Serial.println("left");
  }

  /***************************Right*****************************/
  //If state is equal with letter 'R', wheels will turn right
  else if (state == 'R') {
    digitalWrite(motorA1, LOW);
    digitalWrite(motorA2, LOW);
    digitalWrite(motorB1, LOW);
    digitalWrite(motorB2, HIGH);
    Serial.println("Right");
  }

  /************************Stop*****************************/
  //If state is equal with letter 'S', stop the car
  else if (state == 'S') {
    digitalWrite(motorA1, LOW);
    digitalWrite(motorA2, LOW);
    digitalWrite(motorB1, LOW);
    digitalWrite(motorB2, LOW);
  }


  /*************************location*******************************/

  if (state == 'G')
  {
    Serial.println("SMS Sent");
    get_gps();
    show_coordinate();
    Serial.println("Sending SMS");
    Send();
    Serial.println("SMS Sent");
    delay(2000);

  }

}


void gpsEvent()
{
  gpsString = "";
  while (1)
  {
    while (gps.available() > 0)          //Serial incoming data from GPS
    {
      char inChar = (char)gps.read();
      gpsString += inChar;                   //store incoming data from GPS to temparary string str[]
      i++;
      // Serial.print(inChar);
      if (i < 7)
      {
        if (gpsString[i - 1] != test[i - 1])    //check for right string
        {
          i = 0;
          gpsString = "";
        }
      }
      if (inChar == '\r')
      {
        if (i > 60)
        {
          gps_status = 1;
          break;
        }
        else
        {
          i = 0;
        }
      }
    }
    if (gps_status)
      break;
  }
}


void get_gps()
{

  gps_status = 0;
  int x = 0;
  while (gps_status == 0)
  {
    gpsEvent();
    int str_lenth = i;
    coordinate2dec();
    i = 0; x = 0;
    str_lenth = 0;
  }
}



void show_coordinate()
{

  Serial.print("Latitude:");
  Serial.println(latitude);
  Serial.print("Longitude:");
  Serial.println(logitude);
  Serial.print("Speed(in knots)=");
  Serial.println(Speed);
  delay(2000);

}



void coordinate2dec()
{
  String lat_degree = "";
  for (i = 20; i <= 21; i++)
    lat_degree += gpsString[i];

  String lat_minut = "";
  for (i = 22; i <= 28; i++)
    lat_minut += gpsString[i];

  String log_degree = "";
  for (i = 32; i <= 34; i++)
    log_degree += gpsString[i];

  String log_minut = "";
  for (i = 35; i <= 41; i++)
    log_minut += gpsString[i];

  Speed = "";
  for (i = 45; i < 48; i++)   //extract longitude from string
    Speed += gpsString[i];

  float minut = lat_minut.toFloat();
  minut = minut / 60;
  float degree = lat_degree.toFloat();
  latitude = degree + minut;

  minut = log_minut.toFloat();
  minut = minut / 60;
  degree = log_degree.toFloat();
  logitude = degree + minut;
}



void Send()
{
  gsm.println("AT");
  delay(500);
  serialPrint();
  gsm.println("AT+CMGF=1");
  delay(500);
  serialPrint(); 
  gsm.print("AT+CMGS=");
  gsm.print('"');
  gsm.print("8087809199");    //mobile no. for SMS alert
  gsm.println('"');
  delay(500);
  serialPrint();
  gsm.print("Latitude:");
  gsm.println(latitude);
  delay(500);
  serialPrint();
  gsm.print(" longitude:");
  gsm.println(logitude);
  delay(500);
  serialPrint();
  gsm.print(" Speed:");
  gsm.print(Speed);
  gsm.println("Knots");
  delay(500);
  serialPrint();
  gsm.print("http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=");
  gsm.print(latitude, 6);
  gsm.print("+");              
  gsm.print(logitude, 6);
  gsm.write(26);
  delay(2000);
}



void serialPrint()
{
  while (gsm.available() > 0)
  {
    Serial.print(gsm.read());
  }
}
