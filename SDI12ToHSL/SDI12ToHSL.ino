
///Sketch written by Jeff Sadler ///

#include <SD.h>
const int chipSelect = 10;

/*Note:This code is used for Arduino 1.0 or later*/
//serial communication to Sim900
#include <SoftwareSerial.h>
SoftwareSerial Sim900Serial(2, 3);

#include <SDI12.h>
#define DATAPIN 9         // change to the proper pin
SDI12 mySDI12(DATAPIN); 

//define variables for HydroServer Lite
byte SourceID = 6;
byte SiteID = 84;
byte VarID1 = 10;
byte VarID2 = 11;
String url = "http://worldwater.byu.edu/jefftest.php";

//define variables
char character;

//String Var1 = "55.55";
//String Var2 = "66.66";

byte minInt = 0;
byte secInt = 0;
byte hourInt = 0;
/*byte yearInt = 0;
byte monthInt=0;
byte dayInt=0;*/

char tempBuf[3];

byte recTime = 30;
byte recIntv = 45;

char dateTimeBuf[25];

int count = 0;

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
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
     //don't do anything more:
    }
  else {Serial.println("card initialized.");}
     
 mySDI12.begin();
 
 timestamp();
 timestamp();
 powerUp();
 recTime = secInt + 5;
   if(recTime  >=  60){
      recTime = recTime-60;
      }
}
void loop()
{    
  //begin each loop with timestamp 
  timestamp();
  Serial.println(dateTimeBuf);

  //take measurement when minutes is recTime (e.g. 15)
 if (secInt==recTime){
   recTime += recIntv; 
   sd();  
  
  if(recTime  >=  60){
      recTime = recTime-60;
      }
 }
 if (Serial.available())
    switch(Serial.read())
   {
      case 'm':
      sd();
      break;
     
   } 
 
    
   
   
 delay (300);
     
}

//used to adjust time and date for realtime clock on sim900
/*void Clock(){
  Sim900Serial.println("AT");
  delay(100);
  ShowSerialData();
  Sim900Serial.println("AT+CCLK=\"14/03/25,10:04:30-24\"");
  delay(100);
  ShowSerialData();
}*/

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
void test(){
  Serial.println("test");
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
  String temp = "";
  
  temp = content.substring(slashPosition +4,slashPosition +6);
  temp.toCharArray(tempBuf,3);
  hourInt = atoi (tempBuf);
  
  temp = content.substring(slashPosition +7,slashPosition +9);
  temp.toCharArray(tempBuf,3);
  minInt = atoi (tempBuf);
  
  temp = content.substring(slashPosition +10,slashPosition +12);
  temp.toCharArray(tempBuf,3);
  secInt = atoi (tempBuf);
  
  dateTime.toCharArray(dateTimeBuf, 25);  

}


//writes all values to sd card
void sd(){
  String str = sensors();
   
  int comPos = str.indexOf(",");
  int comPos1 = str.indexOf(",",comPos+1);
  String Var1 = str.substring(comPos+1, comPos1);
  
  int comPos2 = str.indexOf(",",comPos1+1);
  int comPos3 = str.indexOf(",",comPos2+1);
  String Var2 = str.substring(comPos2+1, comPos3);
  
  Serial.println(Var1);
  Serial.println(Var2);
 
 File dataFile = SD.open("test.txt", FILE_WRITE);
  if (dataFile){                                        //do this part each time
    dataFile.print(dateTimeBuf);
    dataFile.print(",");
    dataFile.print(Var1);
    dataFile.print(",");
    dataFile.print(Var2);
    dataFile.print(";");
    dataFile.close();
    Serial.println ("SD Success");
    count ++;
    Serial.print("count: ");
    Serial.println(count);
}
 else{
 Serial.println("error with SD card");
 }
  if(count == 4){
    String dataString = "";
    File dataFile = SD.open("test.txt", FILE_READ);
    if (dataFile) {
      int i=0;
      
      dataFile.seek(dataFile.size()-264);
      Serial.println(dataFile.position());
      while (dataFile.available()&& dataFile.position()<dataFile.size()) {
         char c = dataFile.read();
         dataString += c; 
       }
      dataFile.close();
    } 
    else {
  	// if the file didn't open, print an error:
       Serial.println("error opening test.txt");
    }
    Serial.println(dataString);
    SubmitHttpRequest(dataString);
    count = 0;
 }
} 

///SubmitHttpRequest()
///this function is submit a http request
///attention:the time of delay is very important, it must be set enough 
void SubmitHttpRequest(String dataString)
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
 
  Sim900Serial.print("AT+HTTPPARA=\"URL\",\"http://worldwater.byu.edu/jefftest.php?info=");// setting the httppara, the second parameter is the website you want to access
  Sim900Serial.print(SourceID);
  Sim900Serial.print(",");
  Sim900Serial.print(SiteID);
  Sim900Serial.print(",");
  Sim900Serial.print(VarID1);
  Sim900Serial.print(",");
  Sim900Serial.print(VarID2);
    
  Sim900Serial.print("&readings=");
  Sim900Serial.print(dataString);
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

String sensors(){
  mySDI12.sendCommand("0M7!"); 
  delay(300);                     // wait a while for a response
  String waitStr = bufferToStr();
  String waitTrim = waitStr.substring(0,3);
  int waitInt = waitTrim.toInt()*1000;
  delay(waitInt); // print again in three seconds
  mySDI12.sendCommand("0D0!"); 
  delay(300);                     // wait a while for a response
  String mStr = bufferToStr();
  return(mStr);
}

String bufferToStr(){
  String buffer = "";
  mySDI12.read(); // consume address
  
  while(mySDI12.available()){
    char c = mySDI12.read();
    if(c == '+' || c == '-'){
      buffer += ',';   
      if(c == '-') buffer += '-'; 
    } 
    else {
      buffer += c;  
    }
    delay(100); 
  }
 return(buffer);
}













