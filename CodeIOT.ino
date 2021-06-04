#include <Adafruit_Sensor.h>     
#include <DHT.h>             
#include <WiFi.h>        
#include "Adafruit_MQTT.h"        
#include "Adafruit_MQTT_Client.h" 
#include <Wire.h>
#include <SPI.h>
#include "Adafruit_BME680.h"

// Yasser 

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME680 bme;

// Variable sleep ESP 32 

#define uS_TO_S_FACTOR 1000000ULL  
#define TIME_TO_SLEEP  120        


// Variable nécessaire au bon fonctionnement du WiFi

#define WLAN_SSID       "SSID_ICI"   //nom SSID dde votre rÃ©seau WIFI
#define WLAN_PASS       "MOT_DE_PASSE_ICI" // Clef Wifi

// Variable nécessaire au bon fonctionnement de Adafruit

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "AIO_USERNAME"                       //Votre nom d'utilisateur
#define AIO_KEY         "AIO_KEYi"  //Votre clef AdafruitIO 

// Création des objets WiFi et Adafruit

WiFiClient client;                                                                      
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);  

// Création des flux de données

Adafruit_MQTT_Publish temp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature"); 
Adafruit_MQTT_Publish hum = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidite");     
Adafruit_MQTT_Publish bright = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/luminosite"); 
Adafruit_MQTT_Publish gas = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/gas"); 
Adafruit_MQTT_Publish pression = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/pression"); 
Adafruit_MQTT_Publish altitude = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/altitude"); 


// Configuration des capteurs

#define DHTPINTEMPERATUREHUMIDITE 4  // Définition du PIN D4 pour récupérer la température et l'humidité      
#define DHTTYPE DHT11  // Type de capteur DHT ici le DHT11
DHT dht(DHTPINTEMPERATUREHUMIDITE, DHTTYPE); //Création de l'objet DHT nous permettant ensuite de récupérer les valeurs humidité et température

// Création de valeur time pour la loop
unsigned long myTime1; 
unsigned long myTime2;
unsigned long myTimeFinal;

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO\n"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL\n"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup suite au temps définis de basse consommation.\n"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad\n"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program\n"); break;
    default : Serial.printf("\nWakeup car lancement du programme: %d\n\n",wakeup_reason); break;
  }
}   

void setup() {
  Serial.begin(9600);                    

  WiFi.begin(WLAN_SSID, WLAN_PASS);         //Connexion au WiFi avec les données écrités plus haut 
  while (WiFi.status() != WL_CONNECTED) {   //Tant que le status WiFi n'est pas indiqué comme connecté, continuer d'essayer
    delay(200); 
    Serial.print(".");
  }
  Serial.print(WiFi.status()); 
  Serial.println();
  Serial.println("WiFi connecte");
  Serial.println("IP addresse: "); Serial.println(WiFi.localIP()); //Affichage de l'adresse IP dans le terminal console
  Serial.println("\n\n\n"); 
  dht.begin();  // Démarrage captation 

  while (!Serial);
  Serial.println(F("BME680 async test"));

  if (!bme.begin()) {
    Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
    while (1);
  }

  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150);

}

void loop() {
  // Donne la raison pour laquelle le module RSP32 s'est réveillé
  print_wakeup_reason();
  
  // Début de captation des données
  myTime1 = millis();
  Serial.print("Début de captation des données à : ")  ; 
  Serial.print(myTime1) ;
  Serial.println(" ms.");

  // Connection à MQTT d'adafruitIO
  mqtt.connect();                         

  // Les données
  int potValue = 0;
  const int potPin = 34;
  potValue = analogRead(potPin);
  float temp_val = dht.readTemperature();   
  float hum_val = dht.readHumidity();
  int brightness = map(potValue, 0, 1000, 0, 100);

  // Les données du BME 
  
  unsigned long endTime = bme.beginReading();
  if (endTime == 0) {
    Serial.println(F("Failed to begin reading :("));
    return;
  }
  if (!bme.endReading()) {
    Serial.println(F("Failed to complete reading :("));
    return;
  }

  // Print les différentes données obtenues
  Serial.print("La pression est de : ");
  Serial.print(bme.pressure / 100.0);
  Serial.println(" hPa");

  Serial.print("La présence de gaz est :  ");
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(F(" KOhms"));

  Serial.print("L'altitude approximative est de : ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA) + 80);
  Serial.println(" m");
  
  Serial.print("La température captée est de : ");
  Serial.print(temp_val);
  Serial.println(" degrés celsius.");
  
  Serial.print("L'humidité captée est de : ");
  Serial.print(hum_val);
  Serial.println("%.");
  
  Serial.print("La luminosité captée est de : ");
  Serial.print(brightness);
  Serial.println(" lux.");

  // Print les temps de captations de données.
  myTime2 = millis();
  Serial.print("Fin de transmission des données captées à : ")  ; 
  Serial.print(myTime2);  
  Serial.println(" ms.");
  myTimeFinal = myTime2 - myTime1 ;
  Serial.print("Temps de captation des données : ");
  Serial.print(myTimeFinal);
  Serial.println(" ms.");

  // Publicationd des données sur Adafruit.
  
  Serial.print("WiFi status : ");
  Serial.println(WiFi.status());

  if(WiFi.status() == WL_CONNECTED){
    temp.publish(temp_val);
    delay(3000); 
    //Serial.println("Valeur température publiée sur Adafruit.")  ;                
    hum.publish(hum_val);
    delay(3000);  
    //Serial.println("Valeur humidité publiée sur Adafruit.")  ;
    bright.publish(brightness);
    delay(3000);
    //Serial.println("Valeur luminosité publiée sur Adafruit.")  ;
    gas.publish(bme.gas_resistance / 1000.0);
    delay(3000);
    //Serial.println("Valeur gas publiée sur Adafruit.")  ;
    pression.publish(bme.pressure / 100.0);
    delay(3000);
    //Serial.println("Valeur pression publiée sur Adafruit.")  ;
    altitude.publish(bme.readAltitude(SEALEVELPRESSURE_HPA) +80);
    delay(3000); 
    Serial.println("Fin de transmission des données à Adafruit.");  
  }
  else{
    Serial.println("Aucunes valeurs n'ont pu être transmises à Adafruit dû à un code erreur du WiFi");
  }

  // Light sleep mode définis dans la variable TIME_TO_SLEEP 
    
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); 
  Serial.println("\nLancement du light-sleep mode.\n\n");
  Serial.flush(); 
  esp_light_sleep_start();
  delay(5000); // Delais pour laisser le temps au module de se lancer correctement

  // Reconnexion au WiFi suite à la déconnexion dû au light sleep mode
  Serial.println(WiFi.status());
  WiFi.begin(WLAN_SSID, WLAN_PASS);         //Connexion au WiFi avec les données écrités plus haut 
  while (WiFi.status() != WL_CONNECTED) {   //Tant que le status WiFi n'est pas indiqué comme connecté, continuer d'essayer
    delay(500); 
    Serial.print(".");
  }
  

           
}
