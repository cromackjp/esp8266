
//----------------- Bibliotecas utilizadas ---------------------------------//
// Biblioteca Sensor de Temperatura //
#include <OneWire.h>
#include <DallasTemperature.h>

// Biblioteca ESP8266 Serial //
#include <SoftwareSerial.h>

//------------------ Pinos do ARDUINO utilizado pela ESP8266 - PIN 8 RX, PIN 9 TX
SoftwareSerial espSerial =  SoftwareSerial(8,9); 

//------------------ Pinos conectados no Arduino --------------------//
#define sensortemp 10 
#define ledverm 7
#define ledverd 6
#define cooler 11

//--------------- Temperatura Maxima/Minima----------------------//
#define tempmax 27
#define tempmin 23

OneWire oneWire(sensortemp);

DallasTemperature sensors(&oneWire);

String apiKey = "UV5F6S23VYHZO2WJ";     // PIN 

String ssid="mqtt";    // Wifi SSID
String password ="root@mqtt";  // Wifi password

boolean DEBUG=true;

//======================================================================== showResponce
void showResponse(int waitTime){
    long t=millis();
    char c;
    while (t+waitTime>millis()){
      if (espSerial.available()){
        c=espSerial.read();
        if (DEBUG) Serial.print(c);
      }
    }
                   
}

//========================================================================
boolean thingSpeakWrite(float value1){
  String cmd = "AT+CIPSTART=\"TCP\",\"";                  // Conexão TCP
  cmd += "184.106.153.149";                                   // api.thingspeak.com
  cmd += "\",80";
  espSerial.println(cmd);
  if (DEBUG) Serial.println(cmd);
  if(espSerial.find("Error")){
    if (DEBUG) Serial.println("AT+CIPSTART error");
    return false;
  }
  
  
  String getStr = "GET /update?api_key=";   // prepare GET string
  getStr += apiKey;
  getStr +="&field1=";
  getStr += String(value1);
  getStr += "\r\n\r\n";

  // Enviando dados
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  espSerial.println(cmd);
  if (DEBUG)  Serial.println(cmd);
  
  delay(100);
  if(espSerial.find(">")){
    espSerial.print(getStr);
    if (DEBUG)  Serial.print(getStr);
  }
  else{
    espSerial.println("AT+CIPCLOSE");
    // alert user
    if (DEBUG)   Serial.println("AT+CIPCLOSE");
    return false;
  }
  return true;
}

void setup() {     
          
pinMode(ledverm, OUTPUT);
  pinMode(ledverd, OUTPUT);
  pinMode(cooler, OUTPUT);
      
  DEBUG=true;           // Habilita o debug da serial
  Serial.begin(9600);  
    sensors.begin();          // Inicia o Sensor serial  
  espSerial.begin(9600);  // Habilita serial 
  showResponse(1000);
  espSerial.println("AT+CWJAP=\""+ssid+"\",\""+password+"\"");  // Conecta no AP passando como paramentro o usuário e senha
  showResponse(5000);
   if (DEBUG)  Serial.println("Configuração Realizada com Sucesso !!!");
}


void loop() {
 // ------------------- Le as informações do sensor de temperatura ---------------------- 
      sensors.requestTemperatures();
      #define temp (sensors.getTempCByIndex(0))
  
  
 // -------------------- Condição de funcionamento dos sensores ---------------------- 
  if (temp>tempmax)
  { 
       //Serial.print("\r\nTEMPERATURA ACIMA DO NORMAL: ");
        digitalWrite(ledverd, LOW);
        digitalWrite(ledverm, HIGH);
        digitalWrite(cooler, HIGH);
  }else
  {
        //Serial.print("\r\nTEMPERATURA AMBIEMTE: ");
        digitalWrite(ledverd, HIGH);  
        digitalWrite(ledverm, LOW);
        digitalWrite(cooler, LOW);
  }
  if (temp<tempmin )
  {
    digitalWrite(ledverm, HIGH);
    delay (5000);
    digitalWrite(ledverm, LOW);
    delay (1000);
    digitalWrite(ledverm, HIGH);
    delay (5000);
    digitalWrite(ledverm, LOW);
    delay (1000);
    digitalWrite(ledverm, HIGH);
    delay (5000);
    digitalWrite(ledverm, LOW);
    delay (1000);
    digitalWrite(ledverm, HIGH);
    delay (5000);
    
  }
         float t = temp;
 //------------------- Chama a função Thing ------------------- // 
      thingSpeakWrite(t);
  
  }





