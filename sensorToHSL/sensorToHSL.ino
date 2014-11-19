
///Sketch written by Jeff Sadler ///

#include <SD.h>
const int chipSelect = 10;
#include <DHT22.h>
#define DHT22_PIN 9


/*Note:This code is used for Arduino 1.0 or later*/
//serial communication to Sim900
#include <SoftwareSerial.h>
SoftwareSerial Sim900Serial(2, 3);


//define variables for HydroServer Lite
byte SourceID = 6;
byte SiteID = 84;
byte VarID1 = 10;
byte VarID2 = 11;
String url = 'http://worldwater.byu.edu/interactive/sandbox/parsetest.php';

//define variables
char character;

int Var1 = 2;
byte Var2 = 0;

byte minInt = 0;
byte secInt = 0;
byte hourInt = 0;
byte yearInt = 0;
byte monthInt=0;
byte dayInt=0;

char tempBuf[3];

byte recTime = 30;
byte recIntv = 15;

int bufVar1[8];
byte bufVar2[8];
//char dateTimeBuf[25];

byte bufYear[8];
byte bufMonth[8];
byte bufDay[8];
byte bufHour [8];
byte bufMin [8];

int i=0;


// Setup a DHT22 instance
DHT22 myDHT22(DHT22_PIN);




void setup()
{
  Sim900Serial.begin(115200);               // the GPRS baud rate  
  delay(500);
  Sim900Serial.println("AT+IPR=19200");
  delay(500);
  Sim900Serial.begin(19200);               // the GPRS baud rate   
  delay(1000);
  Serial.begin(9600);               // the Hardware serial rate 
  pinMode(10, OUTPUT);                 //set pinmode 10 as output for SD card
  Serial.println("Please type 's' to send  SMS");
  Serial.println("'d' for timestamp"); 
  Serial.println("'w' to write to SD");
  Serial.println("'t' to write sensor data");
  Serial.println("'tcp' to send data to server");
  Serial.println("or 'h' to submit http request");

  
  
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    }
  else {Serial.println("card initialized.");}
 timestamp();
 timestamp();
 powerDown();
}
void loop()
{    
  //begin each loop with timestamp 
  timestamp();
  Serial.println(secInt);

  //take measurement when minutes is recTime (e.g. 15)
 if (minInt==recTime){
    //get readings and write to sd card
    sensors();
    sd(); 
    
    //add timestamp and reading data to arrays
    bufYear[i] = yearInt;
    bufMonth[i] = monthInt;
    bufDay[i] = dayInt;
    bufHour[i] = hourInt;
    bufMin[i] = minInt;
    bufVar2[i] = Var2;
    bufVar1[i] = Var1;
  
    //scaffolding  
    Serial.print("i= ");
    Serial.println(i);
   
    recTime += recIntv;
    i++;
    //on fourth measurement submit the http request and push measurements along in arrays
    if(i>3){  
      powerUp();
      delay (5000);
      SubmitHttpRequest();
      //SendTextMessage();
      powerDown();
      
      for(i=0;i<4; i++){
       
        bufYear[i+4] = bufYear[i];
        bufMonth[i+4] = bufMonth[i];
        bufDay[i+4] = bufDay[i];
        bufHour[i+4] = bufHour[i];
        bufMin[i+4] = bufMin[i];
        bufVar2[i+4] = bufVar2[i];
        bufVar1[i+4] = bufVar1[i];
      }
      i = 0;
    }
  
  if(recTime  >=  60){
      recTime = recTime-60;
      }
}
 
 

  //scaffolding
  if (Serial.available())
    switch(Serial.read())
   {
      case 'c':
      Clock();
      break; 
      case 's':
      SendTextMessage();
      break;
      case 'd':
      powerDown();
      break;
      case'u':
      powerUp();
      break;
      case 'p':
      powerStatus();
      break;
      case 't':
      sensors();
      break;
      case 'w':
      sd();
      break;
      case 'h':
      SubmitHttpRequest();
      break;
      //case 'tcp':
      //GPRS_SendText();
      //break;
    }
    
   
   
 delay (300);
     
}

//used to adjust time and date for realtime clock on sim900
void Clock(){
  Sim900Serial.println("AT");
  delay(100);
  ShowSerialData();
  Sim900Serial.println("AT+CCLK=\"14/03/25,10:04:30-24\"");
  delay(100);
  ShowSerialData();
}

//powers down sim900
void powerDown(){
  Sim900Serial.println("AT");
  delay(100);
  ShowSerialData();
  Sim900Serial.println("AT+CFUN=0");
  delay(100);
  ShowSerialData();
}

//powers up sim900
void powerUp(){
  Sim900Serial.println("AT");
  delay(100);
  ShowSerialData();
  Sim900Serial.println("AT+CFUN=1");
  delay(100);
  ShowSerialData();
}

//reads whether sim900 is powered up or down
void powerStatus(){
  Sim900Serial.println("AT");
  delay(100);
  ShowSerialData();
  Sim900Serial.println("AT+CFUN?");
  delay(100);
  ShowSerialData();
}

//sends text message
void SendTextMessage()
{  
  Sim900Serial.print("AT+CMGF=1\r");    //Sending the SMS in text mode
  delay(100);
  Sim900Serial.println("AT + CMGS = \"18012315581\"");//The target phone number
  delay(100);
  //Sim900Serial.println(dateTimeBuf);
  Sim900Serial.print(",");
  Sim900Serial.print(Var1);
  Sim900Serial.print("C");
  Sim900Serial.print(",");
  Sim900Serial.print(Var2);
  Sim900Serial.println("%"); //the content of the message
  delay(100);
  Sim900Serial.println((char)26);//the ASCII code of the ctrl+z is 26
  delay(100);
  Sim900Serial.println();
  timestamp();
}
  
//function returns timestamp
void timestamp(){ 
  String content = "";
  Sim900Serial.print("AT + CCLK?\r");
  delay(100);
  while (Sim900Serial.available()){
  character = Sim900Serial.read();
  content.concat(character); 
  content.trim();}
  
  int slashPosition = content.lastIndexOf("/");
  String dateTime = content.substring(slashPosition -5,slashPosition +12);
  String temp = content.substring(slashPosition -5,slashPosition -3 );
  temp.toCharArray(tempBuf,3);
  yearInt = atoi (tempBuf);
  
  temp = content.substring(slashPosition -2,slashPosition -0);
  temp.toCharArray(tempBuf,3);
  monthInt = atoi (tempBuf);
  
  temp = content.substring(slashPosition +1, slashPosition +3);
  temp.toCharArray(tempBuf,3);
  dayInt = atoi (tempBuf);
  
  temp = content.substring(slashPosition +4,slashPosition +6);
  temp.toCharArray(tempBuf,3);
  hourInt = atoi (tempBuf);
  
  temp = content.substring(slashPosition +7,slashPosition +9);
  temp.toCharArray(tempBuf,3);
  minInt = atoi (tempBuf);
  
  temp = content.substring(slashPosition +10,slashPosition +12);
  temp.toCharArray(tempBuf,3);
  secInt = atoi (tempBuf);
  
  //dateTime.toCharArray(dateTimeBuf, 25);  

}

//reads dht22 temperature and humidity values
void sensors(){
 DHT22_ERROR_t errorCode;
  
  // The sensor can only be read from every 1-2s, and requires a minimum
  // 2s warm-up after power-on.
  delay(2000);
  
  //Serial.print("Requesting data...");
  errorCode = myDHT22.readData();
  switch(errorCode)
  {
  case DHT_ERROR_NONE:
  Var1 = myDHT22.getTemperatureC();
  Var2 = myDHT22.getHumidity();
  Serial.print ("Sensor Success");
  Serial.print (Var1);
  Serial.print(",");
  Serial.print(Var2);
  }
  
}

  

//writes all values to sd card
void sd(){
 File dataFile = SD.open("datalog1.txt", FILE_WRITE);
  if (dataFile){
    dataFile.print(yearInt);
    dataFile.print("/");
    dataFile.print(monthInt);
    dataFile.print("/");
    dataFile.print(dayInt);
    dataFile.print(",");
    dataFile.print(hourInt);
    dataFile.print(":");
    dataFile.print(minInt);
    dataFile.print(",");
    dataFile.print(Var2);
    dataFile.print(",");
    dataFile.println(Var1);
    dataFile.close();
    Serial.println ("SD Success");
}
 else{
 Serial.println("error with SD card");
 }
} 

///SubmitHttpRequest()
///this function is submit a http request
///attention:the time of delay is very important, it must be set enough 
void SubmitHttpRequest()
{
  Sim900Serial.println("AT+CSQ");
  delay(100);
 
  ShowSerialData();// this code is to show the data from gprs shield, in order to easily see the process of how the gprs shield submit a http request, and the following is for this purpose too.
 
  Sim900Serial.println("AT+CGATT?");
  delay(100);
 
  ShowSerialData();
 
  Sim900Serial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");//setting the SAPBR, the connection type is using gprs
  delay(1000);
 
  ShowSerialData();
 
  Sim900Serial.println("AT+SAPBR=3,1,\"APN\",\"CMNET\"");//setting the APN, the second need you fill in your local apn server
  delay(4000);
 
  ShowSerialData();
 
  Sim900Serial.println("AT+SAPBR=1,1");//setting the SAPBR, for detail you can refer to the AT slashnd mamual
  delay(2000);
 
  ShowSerialData();
 
  Sim900Serial.println("AT+HTTPINIT"); //init the HTTP request
 
  delay(2000); 
  ShowSerialData();
 
  Sim900Serial.print("AT+HTTPPARA=\"URL\",\""+url+"?info=");// setting the httppara, the second parameter is the website you want to access
  Sim900Serial.print(SourceID);
  Sim900Serial.print(",");
  Sim900Serial.print(SiteID);
  Sim900Serial.print(",");
  Sim900Serial.print(VarID1);
  Sim900Serial.print(",");
  Sim900Serial.print(VarID2);
    
  Sim900Serial.print("&readings=");
  for (int j=0; j<8; j++){
  Sim900Serial.print(bufYear[j]);
  Sim900Serial.print("/");
  Sim900Serial.print(bufMonth[j]);
  Sim900Serial.print("/");
  Sim900Serial.print(bufDay[j]);
  Sim900Serial.print(",");
  Sim900Serial.print(bufHour[j]);
  Sim900Serial.print(":");
  Sim900Serial.print(bufMin[j]);
  Sim900Serial.print(",");
  Sim900Serial.print(bufVar1[j]);
  Sim900Serial.print(",");
  Sim900Serial.print(bufVar2[j]);
  Sim900Serial.print(";");
  }
  Sim900Serial.println("\"");
  
  delay(1000);  
  ShowSerialData();
 
  Sim900Serial.println("AT+HTTPACTION=0");//submit the request 
  delay(10000);//the delay is very important, the delay time is base on the return from the website, if the return datas are very large, the time required longer.
  //while(!Sim900Serial.available());
 
  ShowSerialData();
 
  Sim900Serial.println("AT+HTTPREAD");// read the data from the website you access
  delay(300);
 
  ShowSerialData();
 
  Sim900Serial.println("");
  delay(100);
}



void ShowSerialData()
{
  while(Sim900Serial.available()!=0)
    Serial.write(Sim900Serial.read());
}














