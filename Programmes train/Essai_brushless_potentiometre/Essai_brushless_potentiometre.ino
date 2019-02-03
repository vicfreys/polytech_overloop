/* Programme de test de contrôle d'un moteur brushless à l'aide d'un potentiomètre */

/*
 * Ameliorations :
 *  - Afficher les println sur le LCD
 *  - Essayer avec le levier, vérifier que la transition de l'état BAS à HAUT se fait sans prolèmes
 *  - Programmer par fonctions
  */
/******************************************************/
// Les bibliothèques à inclure au projet
#include <Servo.h> // Inclusion de la bibliothèque Servo : https://www.arduino.cc/en/Reference/Servo
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
/******************************************************/
LiquidCrystal_I2C lc(0x27, 16,2);

#define pot A0 // Potentiomètre à l'entrée analogique A0
#define BP 7 // Bouton Poussoir (BP) à l'entrée numérique 7

Servo esc1; // Défini un objet "esc1", correspond à une des deux turbines
Servo esc2; // Défini un objet "esc2", correspond à une des deux turbines

boolean activation = false; // Activation du mode sustentation

unsigned int val_esc1, val_esc2; // Variables pour les valeurs à transmettre aux ESC
/******************************************************/
void setup()
{
  Serial.begin(9600); // Active la liaison série pour le moniteur série
  lcd.init();
  lcd.backlight();
  lcd.print("Initialisation ESC");
  pinMode(BP, INPUT); // Défini le bouton poussoir en entrée (remplacé par un Levier par la suite)
  
  esc1.attach(9); // Reli l'objet "esc1" à la pin n°9 (SEULS les pins 9 et 10 sont utilisables !!)
  esc1.writeMicroseconds(1000); // Initialise à 1000 us (correspond à aucune rotation du moteur)
  
  esc2.attach(10); // Reli l'objet "esc2" à la pin n°10 (SEULS les pins 9 et 10 sont utilisables !!)
  esc2.writeMicroseconds(1000); // Initialise à 1000 us (correspond à aucune rotation du moteur)
  
  delay(5000); // Permet de laisser aux ESC de se "synchroniser" /!\ A TESTER /!\
  lcd.clear();
}
 
void loop()
{   
  esc1.writeMicroseconds(1000); // Initialise à 1000 us (correspond à aucune rotation du moteur)
  esc2.writeMicroseconds(1000); // Initialise à 1000 us (correspond à aucune rotation du moteur)
  
  if(digitalRead(BP)) // Si on lit une impulsion du BP
  {
    activation = true; // On pourra activer la sustentation
    
    Serial.println("BP activé : Activation des turbines");
    lcd.print("Mise en route");
    delay(300); // Correspond au rebond
    lcd.clear();
    //delay(5000); 
  }

  while(activation) // Avec le levier : while(analogRead(levier))
  {
    
    Serial.println("Turbine en fonctionnement");
    lcd.print("Turbine activee");
    int valPot = analogRead(pot); // On lit la valeur du potentiomètre que l'on stocke dans la variable valPot
    
    // Utiliser les formules suivantes pour avoir un "zero" (= environ 1023/2) correspondant à la sustentation: 
    val_esc1 = (1000./1023.)*analogRead(pot) + 1000; // f1(x) = (1000/1023)*x + 1000
    val_esc2 = -1*(1000./1023.)*analogRead(pot) + 2000; // f2(x) = -(1000/1023)*x + 2000

    // Affiche sur l'ecran les valeurs des signaux PPM envoyés à l'ESC
    lcd.setCursor(0,1);
    lcd.print("Val Esc1: ");
    lcd.setCursor(10,1);
    lcd.print(val_esc1);

    lcd.setCursor(0,2);
    lcd.print("Val Esc2: ");
    lcd.setCursor(10,2);
    lcd.print(val_esc2);
    
    esc1.writeMicroseconds(val_esc1);
    esc2.writeMicroseconds(val_esc2);
    
    lcd.clear();
    
    if(digitalRead(BP)) // Supprimer par la suite
    {
      activation = false;
      
      Serial.println("BP activé : Turbine en arrêt");
      lcd.print("Arret turbine");
      delay(300);
      lcd.clear();
    }
  }
    
  //Serial.println(pulseIn(2,HIGH)); // Pour afficher le temps de la pin est à HAUT
}

/*
 * Fonctions pour la suite (eclaircira le programme)
 */

void controleESC(Servo Esc1, Servo Esc2, unsigned int val_Esc1, unsigned int val_Esc2)
{
   Esc1.writeMicroseconds(val_Esc1); 
   Esc2.writeMicroseconds(val_Esc2); 
}

