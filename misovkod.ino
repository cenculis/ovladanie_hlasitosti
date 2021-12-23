#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp>

int ReadPin=A1;
int mic;           // vstup z mikrofonu
int u;             // hladina max hodnot na danych intervaloch
int Umaxval=610;   // pomocna premenna pri urcovani u - nastavena na hodnotu nulovej hladiny   
int cass=A5;    // pomocna premenna pre hornu hranicu 
int hladinaa=A4;    // pomocna premenna pre spodnu hranicu 
int hlasitost=600; // pomocna premenna na simulovanie a vypisanie pridavania a uberania hlasitosti
int cas;        // horna hranica ktora sa nastavuje pomocou potenciometra
int hladina;// spodna hranica ktora sa nastavuje pomocou potenciometra
int maxval;
int minval;

//premenne pre meranie casu o funkcii maxvolume
unsigned long startMillis;  
unsigned long currentMillis;
const unsigned long period1 = 100; //dlzka intervalu, na ktorom bude merat maximalnu hodnotu

//premenne pre meranie casu vo funkcii volume
unsigned long startmill;  //some global variables available anywhere in the program
unsigned long currentmill;
long period2; // dlzka intervalu odozvy na zmenu hlasitosti

void setup() {
  pinMode(ReadPin,INPUT);
  pinMode(hladina,INPUT);
  pinMode(cass,INPUT);
  pinMode(8,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);  //nastavenie pinu pre infracervenu diodu
  IrSender.begin();  //iniciaizacia prenosu pomocou Infracervenej diody 
  Serial.begin(115200);
  startMillis = millis();
  startmill = millis();
  
}

void loop() {
  cas = map(analogRead(cass),0.00,1024.00,500.00,3000.00);  //zmena velkosti premennej pomocou funkcie map
  hladina = map(analogRead(hladinaa),0,1024,0,50);  //zmena velkosti premennej pomocou funkcie map
  mic=analogRead(ReadPin)+100;   // +100 lebo moja nulova hodnota bola 500 a nechcelo sa mi vsetko prepisova :D 
  maxvolume(mic);
  volume(u);
  maxval=600+(hladina*4);
  minval=600+hladina;
  period2= cas;
  
  Serial.print(hlasitost);
  Serial.print(" ");
  Serial.print(550);  // pomocna hranica
  Serial.print(" ");
  Serial.print(650);  // pomocna hranica
  Serial.print(" ");
  Serial.print(maxval); //horna hranica
  Serial.print(" ");
  Serial.print(minval); //spodna hranica
  Serial.print(" ");
  Serial.print(u);      //hladina max hodnot
  Serial.print(" ");
  Serial.println(mic);  
  
 // delay(1);
}

void YieldDelay(unsigned long ms)
{
    auto now = millis();
    while (millis() < (now + ms))    //funkcia na blikanie ledky bez delay
    {
      
    }
}

//funkcia sluzi na urcovanie hladiny maximalnych hodnot - vystup je u (potom neskor to mozme upravit a spravit lokalne premenne)
void maxvolume(int mic) {
  currentMillis = millis();  
  if (mic>Umaxval) {Umaxval=mic;}
  
  if (currentMillis - startMillis >= period1)  //kontroluje interval
  {
     u=Umaxval;
    startMillis = currentMillis;  // "vynulovanie intervalu"
    Umaxval=610;  // nastavime max hodnotu na nulovu hladinu
  }
}

//pridavanie a uberanie hlasitosti    - v mojom pripade je to simulovane hodnotami od 600 do 700 pricom pridava po 10 - len kvoli tomu aby som to videl v zapisovaci
void volume(int u){
  if (u < minval){
     currentmill = millis();
         if (currentmill - startmill >= period2)   // kontroluje ci dana podmienka bude platit aj po casovom intervale  period2
           {
                  digitalWrite(8, HIGH);           //infoledka aby sme vedeli ze fungujeme 
                  IrSender.sendNEC(0x4, 0x2, 0);   //+ zvuk
                  YieldDelay(200);
                  digitalWrite(8, LOW);
             hlasitost=hlasitost+10;
             if (hlasitost==710) {hlasitost=700;}  //kontrola maximalnej hodnoty
             startmill = currentmill;
           }
          
         
  }
if (u > maxval ){
     currentmill = millis();
         if (currentmill - startmill >= period2)  
           {
                digitalWrite(6, HIGH);          //infoledka aby sme vedeli ze fungujeme 
                IrSender.sendNEC(0x4, 0x3, 0);  //- zvuk
                YieldDelay(200);
                digitalWrite(6, LOW);
            hlasitost=hlasitost-10;
             if (hlasitost==590) {hlasitost=600;} //kontrola maximalnej hodnoty
             startmill = currentmill; 
           }
  }
}         
         
  
