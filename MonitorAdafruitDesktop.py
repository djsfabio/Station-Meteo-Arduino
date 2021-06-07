from selenium import webdriver
import time
from termcolor import colored
import discord_notify as dn
from datetime import datetime
import pyfiglet
from webdriver_manager.chrome import ChromeDriverManager


ascii_banner = pyfiglet.figlet_format("MonitorAdafruit", font = "slant")
print(colored(ascii_banner, "cyan"))

print("Veuillez entrer le webhook du chanel Discord concerné.")
notifier = dn.Notifier(str(input()))

print("Veuillez entrer le nombre de secondes entre chaque ping.")
tempsDattente = int(input())

print("Veuillez entrer votre pseudo Adafruit : ")
usernameAdafruit = str(input())

print("Veuillez entrer votre mot de passe Adafruit : ")
passwordAdafruit = str(input())


def main ():
  notifier.send("@everyone Lancement du scan météo via Adafruit toutes les " + str(tempsDattente) + " secondes.", print_message=False)
  pasConnecte = 1  
  options = webdriver.ChromeOptions()
  options.set_headless()
  driver = webdriver.Chrome(ChromeDriverManager().install(), options=options)

  try:
    while True:
      if(pasConnecte == 1):
        driver.get("https://accounts.adafruit.com/users/sign_in")
        driver.find_element_by_id("user_login").send_keys(usernameAdafruit)
        elementMDP = driver.find_element_by_id("user_password")
        elementMDP.send_keys(passwordAdafruit)
        elementMDP.submit()
        time.sleep(5)
        pasConnecte = 0
        driver.get("https://io.adafruit.com/" + usernameAdafruit + "/dashboards")
        time.sleep(5)
        driver.get("https://io.adafruit.com/" + usernameAdafruit + "/feeds")
        time.sleep(5)
        notificationAvecValeur(driver)
      else:  
        driver.refresh()
        time.sleep(10)
        notificationAvecValeur(driver)

  except KeyboardInterrupt:
    notifier.send("--------------------\nArrêt du script\n--------------------", print_message=True)
    pass
  
def notificationAvecValeur(driver):
  elementTBody = driver.find_element_by_tag_name("tbody")
  listElement = elementTBody.find_elements_by_tag_name("tr")
  now = datetime.now()
  date_now = now.strftime("%m/%d/%Y")
  time_now = now.strftime("%H:%M:%S")
  notifier.send("La température donnée a été relevée le " + date_now + " à l'heure suivante : " + time_now, print_message=False)
  print("\nLa température donnée a été relevée le " + colored(date_now,"red") + " à l'heure suivante : " + colored(time_now, "green") + "\n\n\n")
  for i in listElement:
    x = i.text.split(" ")
    communicationDiscord(x)
    time.sleep(2)
  notifier.send("--------------------\nSleep mode pendant "+ str(tempsDattente) + " secondes.\n--------------------", print_message=True)
  
  time.sleep(tempsDattente)

  
def communicationDiscord(x): 
  if(x[0] == "altitude"):
    printAndSend("L'altitude est égale à : ", x[1] + " mètres.\n")
  elif(x[0] == "gas"):
    printAndSend("La présence de gas est égale à : ", x[1] + " KOhms.\n")
  elif(x[0] == "humidite"):
    printAndSend("L'humidité dans l'air est de : ", x[1] + "%.\n")
  elif(x[0] == "luminosite"):
    printAndSend("La luminosité mesurée est de : ", x[1] + " lux.\n")
  elif(x[0] == "pression"):
    printAndSend("La pression est égale à : " , x[1] + " HPa.\n")
  elif(x[0] == "temperature"):
    printAndSend("La température est égale à : ", x[1] + " degrès celsius.\n")
  else:
    notifier.send("Valeur non interprétable.", print_message=False)
    print(colored("Valeur non interprétable.", "red"))
  
def printAndSend(phrase, valeur):
  notifier.send(phrase + valeur, print_message=False)
  print(phrase + colored(valeur, "yellow"))


if __name__ == "__main__":
    main()