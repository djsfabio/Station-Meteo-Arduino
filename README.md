# Projet Architecture & IoT Platform 
## Technologies utilisés :
Un webhook de Discord

Selenium en headless

Aio.adafruit.com


## Systèmes embarqués utilisés : 
Raspberry PI 4B 

Joy-IT SBC-NodeMCU-ESP32

## Capteurs utilisés : 
Photorésistance (LDR)

Velleman VMA311

Bosch BME 680

## Objectif : 

L’objectif de ce projet est de développer une application IoT permettant de collecter des données sur l’envi- ronnement intérieur (température, humidité, pression, luminosité, présence...) les stocker et visualiser sur Adafruit et les publier via webhook sur Discord grâce à Selenium. 

## Contenu de ce Github : 

CodeIOT.ino permettant d'être téléversé dans l'ESP32

MonitorAdafruit.py à lancer directement depuis un Raspberry PI pour un monitoring via un WebHook Discord. 

## Utilisation 

Ouvrir l'IDE Arduino ou l'IDE Visual Studio Code avec le Framework Plateform IO, ne pas oublier de renseigner son SSID et son Password, en plus de l'id et token Adafruit, et téléverser le fichier CodeIOT.ino sur l'ESP32.

Lancement du fichier python sur votre ordinateur ou depuis la Raspberry, sélectionner le fichier qui convient, et ne pas oublier d'ajouter ses login directement dans le fichier : 

```
pip install -r requirements.txt
```
ou
```
pipenv install -r requirements.txt
````
```
pipenv shell
```
puis si vous êtes sur Desktop : 
```
python MonitorAdafruitDesktop.py
```
et si vous êtes sur Raspberry : 
```
python MonitorAdafruitRaspberry.py
```
Suivre ensuite les indications directement sur le terminal. 

# Exemple Discord : 

![alt text](https://github.com/djsfabio/Station-Meteo-Arduino/blob/master/Discord.jpg?raw=true)

# Image du montage final : 

![alt text](https://github.com/djsfabio/Station-Meteo-Arduino/blob/master/ImageMontage.JPG?raw=true)
