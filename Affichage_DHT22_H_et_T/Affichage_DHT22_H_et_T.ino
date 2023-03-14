/*
 Description: Programme permettant d'afficher sur un afficheur liquid-crystal, température et taux d'humidité
              mesurés par un DHT22, branché sur la pin D2.

 Auteur:  Christian Mercier  
 Créé le:  

 Librairies utilisées: DHT sensor library https://github.com/adafruit/DHT-sensor-library
                                          https://github.com/adafruit/DHT-sensor-library                                        
                     : Liquid-crystal-I2C https://www.arduinolibraries.info/libraries/liquid-crystal-i2-c                                                                 
                     : Wire 
                     
 Informations sites:    https://www.volta.ma/comment-utiliser-les-capteurs-dht11-et-dht22-avec-arduino/arduino/
                        https://scolawebtv.crdp-versailles.fr/?id=36132  
                        https://www.youtube.com/watch?v=lJU6JCm94O8                                           
  */
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);// Adresse,lignes,colonnes

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 2 // Pin raccordement DHT22
//#define DHTTYPE DHT11 // DHT 11
#define DHTTYPE DHT22 // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21 // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE); // Instance librairie DHT
  float valeurH = 0.0;
  float valeurT = 0.0;
  float valeurTcorrigee = 0.0;
  float valeurTressentie = 0.0;
  float valeurRessentieEnCelsius = 0.0;
  float var1T = 0.9594; // En fonction de la courbe de tendance
  float var2T = 0.8247; // En fonction de la courbe de tendance

  byte bitDefaut1;
  byte bitDefaut2;
  const int pinLed13 = 13;
//---------------------------------------
void setup ()
{     
  Serial.begin(9600);     
  dht.begin(); // Init DHT22;
  
  lcd.init();// init Afficheur                    
  lcd.backlight();// Active retro-eclairage
  lcd.setCursor(1,0);// Colonne, Ligne
  lcd.print(" Capteur DHT22");
      
  pinMode(pinLed13, OUTPUT);
  digitalWrite(pinLed13, LOW);     

  bitDefaut1 = LOW; 
  bitDefaut2 = LOW;
  
  delay(1000);  
  lcd.clear();// Effacement LCD     
}// fin Setup 
//--------------------------------------------
void loop ()
{
// Appel routine releves temp et humidité du DHT22
  InfosTempEtHumidite(); 
}// Fin LOOP
//--------------------------------------------
void InfosTempEtHumidite()
{  
  float H = dht.readHumidity(); // Humidité en %
  delay(100);
  float T = dht.readTemperature();
  delay(100);
  valeurRessentieEnCelsius = dht.computeHeatIndex(T, H, false); // False = °C
  valeurH = H;
  valeurT = T;
  valeurTressentie = valeurRessentieEnCelsius;
// Calcul T avec courbe de tendance
  valeurTcorrigee = (float)var1T*valeurT-var2T;     
if (isnan(H) || isnan(T))
  {  
    bitDefaut1 = HIGH;
    digitalWrite(pinLed13, HIGH); 
      if (bitDefaut2 == LOW)
        {
          Affichage_defaut_LCD();  
        }
    return;      
  }
else
  {
    bitDefaut1 = LOW;      
      
      if(bitDefaut1 == LOW & bitDefaut2 == HIGH)
        {
          lcd.clear();     
        }    
    bitDefaut2 = LOW;
    digitalWrite(pinLed13, LOW);    
// Appel Routine Affichage H  
    Affichage_H_LCD();
    delay(100);    
// Appel Routine Affichage Tcorrigee    
    Affichage_T_LCD();     
  }    
  Serial.print(valeurT); Serial.print("---");Serial.print(valeurTcorrigee); 
  Serial.println();
  delay(2000); 
}// FIN VOID
//--------------------------------------------
void Affichage_H_LCD()
{
  lcd.setCursor(0,0);// colonne, ligne
  lcd.print("Humidite:");
  lcd.setCursor(11,0);
  lcd.print(valeurH);
  lcd.setCursor(6,1);
  lcd.print("%");    
}// FIN VOID 
//--------------------------------------------
void Affichage_T_LCD()
{
  lcd.setCursor(0,1);// // colonne, ligne 
  lcd.print("Temp:");  
  lcd.setCursor(11,1);  
  lcd.print(valeurT);  
  lcd.setCursor(15,1);
  lcd.print((char)223); //caractere degre    
}// FIN VOID
//--------------------------------------------
void Affichage_defaut_LCD()
{ 
  lcd.clear();
  lcd.setCursor(1,0);// // colonne, ligne
  lcd.print("Defaut capteur");
  lcd.setCursor(0,1);
  lcd.print("----------------");
  bitDefaut2 = HIGH;       
}// FIN VOID 
