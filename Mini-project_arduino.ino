
/*

*/
// Définition des PIN
#define pin_led  6
#define pin_trig 2
#define pin_echo 3
#define pinBouton 10
#define pinBoutonlow 9
#define PinK64 5
#define PinK64light 11

// initialization
int inches = 0;
int val; // Valeur de la LED
int cm1 = 0, cm2 = 0, v = 0; // Distance in cm
int distance_max = 300; //maximum distance we can mesure
int Vmax = 50;

// We calculte in this function the distance from the sensor to an object
long readUltrasonicDistance(int triggerPin, int echoPin)
{
  pinMode(triggerPin, OUTPUT);  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  // Reads the echo pin, and returns the sound wave travel time in microseconds
  return pulseIn(echoPin, HIGH);
}
// main code
void setup()
{
  pinMode(9, OUTPUT); // On déclare la led en mode OUTPUT
  Serial.begin(9600); // on paramètre le moniteur afin d'afficher les valeurs 
  pinMode(pinBouton,INPUT);
  pinMode(pinBoutonlow,INPUT);
  pinMode(PinK64light,INPUT);
}

void loop()
{
  v=0;
  //boolean etatlight = digitalRead(PinK64light);
  boolean etatBouton=digitalRead(pinBouton);
  if (etatBouton == 1){
    Vmax += 5;
  }
 
  boolean etatBoutonlow=digitalRead(pinBoutonlow);
  if (etatBoutonlow == 1){
    Vmax -= 5;
  }
  //if (etatlight == 1){
   // Vmax-=10;
  
  // measure the ping time in cm
  cm1 = 0.01723 * readUltrasonicDistance(2, 3); // Mesure de la distabce
  delay(300);
  v = abs(cm2-cm1)/(0.3);
  cm2 = 0.01723 * readUltrasonicDistance(2, 3); // Mesure de la distabce
  if (v > 3 and v < 300){
    Serial.println("-------------------------------------------");
    Serial.print("Speed : "); // On affiche le résultats+
    Serial.print(v);
    Serial.println(" cm/s");
    Serial.print(v*0.036);
    Serial.println(" km/h");
    Serial.print("Max speed = : ");
    Serial.print(Vmax);
    Serial.println(" cm/s");
    Serial.println("-------------------------------------------");
  }

  
  if (v > Vmax and v < 300){
  
    analogWrite(pin_led,HIGH); // on allume la led avec la bonne valeur.
    Serial.println(" WARNING : MAXIMUM SPEED EXCEED ");
  }
  else{
    
    analogWrite(pin_led,LOW); // on allume la led avec la bonne valeur.

   }

  if (v > 3 and v < 100){
    
    v=map(v,0,99,0,255);
    analogWrite(PinK64, v);
    delay(7);
  }
  delay(1000);
  
  
}
