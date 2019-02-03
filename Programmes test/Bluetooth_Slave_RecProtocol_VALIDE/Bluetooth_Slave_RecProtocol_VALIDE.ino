
// ==================================
//          Bibliothèques
#include <Wire.h>
#include "rgb_lcd.h"

// ==================================
//        Définition des objets
rgb_lcd lcd; // Ibjet LCD

// ==================================
// Définition des variables globales

// ============= define =============

//============== int ================
const int colorR = 0;
const int colorG = 0;
const int colorB = 255;
int activation;
int val_esc1, val_esc2;
//============= byte ===============
const byte numChars = 32;
//============= char ===============
char receivedChars[numChars]; // Chaine du message reçu
char tempChars[numChars]; // Chaine temporaire pour stocker le message
//============= boolean ===============
boolean newData = false;


void setup() {
  //======= LCD =======
  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB);
  lcd.print("Mini Projet");
  delay(1000);
  lcd.clear();
  
  Serial.begin(38400); // Vitesse de communication du Bluetooth
}

void loop() {
  recvWithStartEndMarkers();
  // Si la donnée envoyée est nouvelle
  if (newData == true) {
      strcpy(tempChars, receivedChars);
          // this temporary copy is necessary to protect the original data
          //   because strtok() used in parseData() replaces the commas with \0
      parseData();
      showParsedData();
      newData = false;
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

void showParsedData() {
    lcd.setCursor(0,0);
    lcd.print("A:");
    lcd.print(activation);
    lcd.setCursor(0,1);
    lcd.print(val_esc1);
    lcd.setCursor(6,1);
    lcd.print(val_esc2);
    //delay(50);
    lcd.clear();
}
