/*  Programme de la commande
 * Processus:
 *    1) On active la poly-commande avec le switch (mise sous tension)
 *    2) L'écran LCD affiche un menu simple (nom projet, etc..)
 *    3) L'activation du train se fait lorsque le levier est enclenché:
 *    4) Génération du signal PPM de 1000uS
 *    5) 
 */
// ==================================
//          Bibliothèques
// ==================================
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
// ==================================
//        Définition des objets
// ==================================
LiquidCrystal_I2C lc(0x27, 16,2);
// ==================================
// Définition des variables globales
// ==================================

// ============= define =============
#define joystick A0 // Joystick à l'entrée analogique A0
//============== int ================
unsigned int val_esc1, val_esc2;
//============== boolean ============
volatile boolean activation = false;
//==================================

//    Interruption sur la Pin D13
//    Vecteur d'interruption : PCINT0_vect
//    Registre PINB : PINB5
ISR(PCINT0_vect){
  if(PINB &(1<<PINB5)){
    activation  = !(activation);
  }
}

void setup() {
  //=== Paramétrage des registres ===
  //  Registre PCICR : PCIE0 (D5-D13)
  //  Registre PSMSK0 : PCINT5 (D13)
  PCICR |= (1 << PCIE0);
  PCMSK0 |= (1 << PCINT5);

  Serial.begin(38400); // Vitesse de communication du Bluetooth
  
  //===== LCD ======
  lcd.init();
  lcd.backlight();
  lcd.print("Polycommande");

  delay(1000);
  lcd.clear();
}

void loop() {
  // Utiliser les formules suivantes pour avoir un "zero" (= environ 1023/2) correspondant à la sustentation: 
  val_esc1 = int((1000./1023.)*analogRead(joystick) + 1000); // f1(x) = (1000/1023)*x + 1000
  val_esc2 = int(-1*(1000./1023.)*analogRead(joystick) + 2000); // f2(x) = -(1000/1023)*x + 2000
  protocolCom();
}

//======================================
// ===== Protocole de communication ====
//      Marqueur de début : "<"
//      Marqueur de séparation : ","
//      Marqueur de fin : ">"
void protocolCom(){
  Serial.print("<" + String(activation) + "," + String(val_esc1)+ "," + String(val_esc2) + ">");
  //affichageLCD();
  delay(50); // Impératif pour laisser le temps à l'arduino esclave de recevoir correctement les informations
}

void affichageLCD() {
    lcd.setCursor(0,0);
    lcd.print("A:");
    lcd.print(activation);
    lcd.setCursor(0,1);
    lcd.print(val_esc1);
    lcd.setCursor(6,1);
    lcd.print(val_esc2);
    lcd.clear();
}
