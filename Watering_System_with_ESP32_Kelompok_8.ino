#define BLYNK_TEMPLATE_ID "TMPL6_f5Xc3Q9"
#define BLYNK_TEMPLATE_NAME "Watering System"
#define BLYNK_AUTH_TOKEN "2TjdOu4LrNrJwvo90oMo2OGmOi7mUI37"
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#define DHTPIN 5 
#define DHTTYPE DHT11
#define ONE_WIRE_BUS 4  
#define pump 14
DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire); 

LiquidCrystal_I2C lcd(0x27, 16, 2);
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Redmi A2";
char pass[] = "pepekhalus";

const int AirValue = 2620;   
const int WaterValue = 1180;  
int soilMoistureValue = 0;
int soilmoist=0;  
int humi, temp,fp,sistem; 
int buttonState;
//-- GANTI SESUAI DENGAN NILAI SET POINT ANDA
int SP_LOW=40;
int SP_HIGH=60;
//=============================
void read_DHT11(){
  humi = dht.readHumidity();   
  if (isnan(humi)) {
    Serial.println("DHT11 tidak terbaca... !");
    return;
  }
  else{   
  lcd.setCursor(4,1);
  lcd.print(humi);
  lcd.print("%");
  }  
}
//===========
void read_SoilMoist(){
  soilMoistureValue = analogRead(A6);  
  soilmoist= map(soilMoistureValue, AirValue, WaterValue, 0, 100);
  if(soilmoist >= 100)
    {
      soilmoist=100;
    }
  else if(soilmoist <=0)
    {
      soilmoist=0;
    }
    
  Serial.print("Soil Moisture :");
  Serial.print(soilmoist);
  Serial.println("%");

  lcd.setCursor(4,0);
  lcd.print(soilmoist);   
  lcd.print("% "); 
}
//---------------------
void read_DS1820(){
  sensors.requestTemperatures();  
  temp=sensors.getTempCByIndex(0);  
  lcd.setCursor(12,0);
  lcd.print(temp);   
}
//=================================
BLYNK_WRITE(V3){
  buttonState = param.asInt();
  if(buttonState==HIGH){
    sistem=1;//automatic
    fp=0;
    delay(10);
  }
  else if(buttonState==LOW){
    sistem=0;//manual
    delay(10);    
  }
}
//==========================================
BLYNK_WRITE(V4){
  buttonState = param.asInt();
  if(sistem==0){
    if(buttonState==LOW){
      digitalWrite(pump,HIGH); 
      lcd.setCursor(12,1);
      lcd.print("OFF");
      delay(10);
      }
    else if(buttonState==HIGH){
      digitalWrite(pump,LOW); 
      lcd.setCursor(12,1);
      lcd.print("ON ");
      delay(10);    
      }
  }
}
//==============================
void setup()
{  
  Serial.begin(115200);
  lcd.begin(16,2); 
  lcd.init();  
  lcd.backlight(); 
  lcd.setCursor(0, 0); 
  lcd.print("Kelompok 8");
  lcd.setCursor(0, 1);
  lcd.print("    IoT Project :D   ");
  pinMode(pump,OUTPUT);  
  digitalWrite(pump,HIGH);
  dht.begin();  
  sensors.begin();
  delay(2000);  
  Blynk.begin(auth, ssid, pass);
  lcd.clear();
  lcd.print("Mst=  %   T=   C");    
  lcd.setCursor(0, 1);
  lcd.print("Hum=  %   P=OFF");    
}
//=============================
void loop()
{
  Blynk.run();  
  read_DHT11();
  read_SoilMoist();
  read_DS1820();
  delay(1000);
  Blynk.virtualWrite(V0, temp);
  Blynk.virtualWrite(V1,soilmoist);
  Blynk.virtualWrite(V2,humi);
  //-----control
  if(sistem==1){
    if((soilmoist<SP_LOW)&&(fp==0)){
      digitalWrite(pump,LOW);
      lcd.setCursor(12,1);
      lcd.print("ON ");
      fp=1;
    }
    else if((soilmoist>SP_HIGH)&&(fp==1)){
      digitalWrite(pump,HIGH);
      lcd.setCursor(12,1);
      lcd.print("OFF");
      fp=0;
    }
  }
}
