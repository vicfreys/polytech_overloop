/*  Programme du train
 * Processus:
 *    1) On active la poly-commande avec le switch (mise sous tension)
 *    2) L'écran LCD affiche un menu simple (nom projet, etc..)
 *    3) L'activation du train se fait lorsque le levier est enclenché:
 *    4) Génération du signal PPM de 1000uS
 *    5) 
 */
// ==================================
//          Bibliothèques

//#include <Wire.h>
//#include "rgb_lcd.h"

#include <Servo.h> // Inclusion de la bibliothèque Servo : https://www.arduino.cc/en/Reference/Servo
// ==================================
//        Définition des objets

//rgb_lcd lcd; // Ibjet LCD

Servo esc1; // Défini un objet "esc1", correspond à une des deux turbines
Servo esc2; // Défini un objet "esc2", correspond à une des deux turbines
// ==================================
// Définition des variables globales

// ============= define =============
#define accelero 
//============== int ================

//const int colorR = 0; // LCD rouge
//const int colorG = 0; // LCD vert
//const int colorB = 255; // LCD bleu

int activation; // 0 pour FAUX et 1 pour VRAI
int val_esc1, val_esc2; // Valeur de commande pour les esc [1000;2000]
const unsigned int escMIN = 1000;
const unsigned int escMAX = 2000;
//============= byte ===============
const byte numChars = 32; // Longueur maximale de la chaine de caractère
//============= char ===============
char receivedChars[numChars]; // Chaine du message reçu
char tempChars[numChars]; // Chaine temporaire pour stocker le message
//============= boolean ===============
boolean newData = false; // Variable booléenne pour savoir si un nouveau message est reçu


void setup() {
  //======= LCD =======
  //lcd.begin(16, 2);
  //lcd.setRGB(colorR, colorG, colorB);
  //lcd.print("Mini Projet");
  //delay(1000);
  //lcd.clear();
  
  Serial.begin(38400); // Vitesse de communication du Bluetooth

  esc1.attach(9); // Reli l'objet "esc1" à la pin n°9 (SEULS les pins 9 et 10 sont utilisables !!)
  esc1.writeMicroseconds(1000); // Initialise à 1000 us (correspond à aucune rotation du moteur)
  
  esc2.attach(10); // Reli l'objet "esc2" à la pin n°10 (SEULS les pins 9 et 10 sont utilisables !!)
  esc2.writeMicroseconds(1000); // Initialise à 1000 us (correspond à aucune rotation du moteur)

   delay(3000); // Permet de laisser aux ESC de se "synchroniser"
}

void loop() {
  recvWithStartEndMarkers();
  // Si la donnée envoyée est nouvelle
  if (newData == true) {
      strcpy(tempChars, receivedChars);
          // this temporary copy is necessary to protect the original data
          //   because strtok() used in parseData() replaces the commas with \0
      parseData();
      newData = false;
  }

  if(activation){
    esc1.writeMicroseconds(val_esc1);
    esc2.writeMicroseconds(val_esc2);
  }
  else{
    esc1.writeMicroseconds(escMIN);
    esc2.writeMicroseconds(escMIN);
  }
}

//=====================================================================
// Fonction de réception de données selon le protocole de communication
void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<'; // Marque de début de chaine
    char endMarker = '>'; // Marque de fin de chaine
    char rc;

    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();
        // On a bien reçu '<' en début de chaine, on lance la réception
        if (recvInProgress == true) {
            if (rc != endMarker) { // Tant que la valeur lue n'est pas '>' (correspond à la fin de chaine)
                receivedChars[ndx] = rc; // On stocke la valeur dans le tableau
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }
        // Si la donnée reçue commence par '<'
        else if (rc == startMarker) {
            recvInProgress = true;
        }
        //delay(50);
    }
}
//=====================================================
// Permet de séparer chaque donnée de la chaine envoyée
void parseData() {      // split the data into its parts

    char * strtokIndx; // this is used by strtok() as an index

    strtokIndx = strtok(tempChars,",");      // get the first part - the string
    activation = atoi(strtokIndx);
 
    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    val_esc1 = atoi(strtokIndx);     // convert this part to an integer

    strtokIndx = strtok(NULL, ",");
    val_esc2 = atoi(strtokIndx);     // convert this part to a float
}
