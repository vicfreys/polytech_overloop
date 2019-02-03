/*
 * How to configure and pair two HC-05 Bluetooth Modules
 * by Dejan Nedelkovski, www.HowToMechatronics.com
 * 
 *                 == MASTER CODE ==
 */
// ==================================
//          Bibliothèques
// ==================================

// ==================================
//        Définition des objets
// ==================================

// ==================================
// Définition des variables globales
// ==================================

// ============= define =============
#define pot A0 // Potentiomètre à l'entrée analogique A0
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
}

void loop() {
  // Utiliser les formules suivantes pour avoir un "zero" (= environ 1023/2) correspondant à la sustentation: 
  val_esc1 = int((1000./1023.)*analogRead(pot) + 1000); // f1(x) = (1000/1023)*x + 1000
  val_esc2 = int(-1*(1000./1023.)*analogRead(pot) + 2000); // f2(x) = -(1000/1023)*x + 2000
  protocolCom();
}

//======================================
// ===== Protocole de communication ====
//      Marqueur de début : "<"
//      Marqueur de séparation : ","
//      Marqueur de fin : ">"
void protocolCom(){
  Serial.print("<" + String(activation) + "," + String(val_esc1)+ "," + String(val_esc2) + ">");
  delay(50); // Impératif pour laisser le temps à l'arduino esclave de recevoir correctement les informations
}
