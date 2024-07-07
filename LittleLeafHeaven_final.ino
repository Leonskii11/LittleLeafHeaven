#include <DHT.h>
#include <Adafruit_NeoPixel.h>
#include <RTClib.h>

#define DHTPIN 9       // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22  // Change this to DHT11 if you're using that sensor

//Temperatur/Feuchtigkeitssensor

DHT dht(DHTPIN, DHTTYPE);
const int relaisVent = 5;
const int relaisHeat = 6;
float cvalue;            //für nan-Prüfung
float temperature = 20;  // Read temperature in Celsius

//Bewässerung

const int soilSensorInput = A1;
const int waterLevelInput = 2;
const int pumpActivated = 4;
const int AirValue = 575;
const int WaterValue = 268;
int soilMoistureValue = 0;
int soilMoisturePercentage = 0;
int waterLevel = 0;

//LED

const int LEDStripe = 8;
const int LEDWaterLevel = 3;

const int NUMPIXELSLight = 56;
const int NUMPIXELSWaterLevelCheck = 1;

Adafruit_NeoPixel Light = Adafruit_NeoPixel(NUMPIXELSLight, LEDStripe, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel WaterLevelCheck = Adafruit_NeoPixel(NUMPIXELSWaterLevelCheck, LEDWaterLevel, NEO_GRB + NEO_KHZ800);

RTC_DS1307 rtc;

int OnOff = 0;


void setup() {
  pinMode(waterLevel, INPUT);
  pinMode(pumpActivated, OUTPUT);
  digitalWrite(pumpActivated, HIGH);

  Serial.begin(9600);

  dht.begin();
  pinMode(relaisVent, OUTPUT);
  digitalWrite(relaisVent, HIGH);
  pinMode(relaisHeat, OUTPUT);
  digitalWrite(relaisHeat, HIGH);

  rtc.begin();

  Light.begin();
  WaterLevelCheck.begin();

  delay(2000);
}


void loop() {

  Serial.println(soilMoisturePercentage);
// Read DHT-22

  cvalue = dht.readTemperature();

  if(isnan(cvalue) || cvalue < 0 || cvalue > 40)
  {
    if(digitalRead(relaisHeat) == LOW && digitalRead(relaisVent) == HIGH)
    {
    temperature = 20;
    }
    else if(digitalRead(relaisHeat) == HIGH && digitalRead(relaisVent) == LOW)
    {
    temperature = 25;
    }
    else
    {
    temperature = 22;
    }
  }

  else 
  {
    temperature = cvalue;
  }


// Heater

  if(temperature < 21 && digitalRead(relaisHeat) == HIGH)
    {
        digitalWrite(relaisHeat, LOW);
    }

  if(temperature > 22 && digitalRead(relaisHeat) == LOW)
    {
      digitalWrite(relaisHeat, HIGH);
    }


// Ventilator

  if(temperature > 23 && digitalRead(relaisVent) == HIGH)
    {
        digitalWrite(relaisVent, LOW);
    }

  if(temperature < 23 && digitalRead(relaisHeat) == LOW)
    {
      digitalWrite(relaisVent, HIGH);
    }

Serial.print(temperature);
  // Bewässerung

  waterLevel=digitalRead(waterLevelInput);
  soilMoistureValue = analogRead(soilSensorInput);
  soilMoisturePercentage = map(soilMoistureValue, AirValue, WaterValue, 0, 100);

  if(waterLevel == 0)
  {
    digitalWrite(pumpActivated, HIGH);
  }

  else if (soilMoisturePercentage > 5)
  {
    digitalWrite(pumpActivated, HIGH);
  }
  
  else if (soilMoisturePercentage <=5)
  {       
  digitalWrite(pumpActivated, LOW);
  delay(2000);
  digitalWrite(pumpActivated, HIGH);
  }

  
//LED

  DateTime now = rtc.now();

  if(now.hour() >= 10 && now.minute() >= 0) 
    {
      OnOff = 1;

      if(OnOff == 1)
        {
          for(int n=0; n<56; n++)
            {
              if(n % 4)  
                Light.setPixelColor(n, Light.Color(40, 0, 0));
              else
                Light.setPixelColor(n, Light.Color(0, 0, 40));
            }
        }
    }

  if(now.hour() >= 20 && now.minute() >= 0) 
    {
      OnOff = 0;

      if(OnOff == 0)
        {
          for(int n=0; n<56; n++)
            { 
              Light.setPixelColor(n, Light.Color(0, 0, 0));
            }
        }
    }

  Light.show();

  if(waterLevel == 1)
    {
      WaterLevelCheck.setPixelColor(0, WaterLevelCheck.Color(0, 25, 0));
    }

  else
    {
      WaterLevelCheck.setPixelColor(0, WaterLevelCheck.Color(25, 0, 0));
    }
 
  WaterLevelCheck.show();
  delay(1000);
}
