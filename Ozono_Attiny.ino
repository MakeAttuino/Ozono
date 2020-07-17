/*

  Modulo per l'ozonizzazione dell'aria
  tramite applicazione ottenuta con la
  generazione di ozono, di 0,5mg/h
  purificando un piccolo ambiente privato.
  Componenti utilizzati:
  - Attiny 85 8Mhz
  - display Oled 0,91"
  - 78L05 regolatore in 12V out 5V
  - BC337 per ventola
  - resistenza 1K
  - BS170 per ozonizzatore
  - resistenza 10K
  - pulsante
  - ventola 12V
  - generatore ozono 0,5mg

  ************************************

  Progetto:    Ozonatore ambiente
  Versione:    V.1.0
  Data:        30/03/2020
  Autore:      L.M
  
*/

// Parametri per display oled 0.96"
#include "SSD1306_minimal.h"
#include <avr/pgmspace.h>
SSD1306_Mini oled;

// porte I/O
#define ozono PB4
#define ventola PB3
#define start_stop PB1

// sistema
uint8_t stato = 0;          // stato della macchina
uint8_t max_stato = 4;
#define durata_ventola 20
#define durata_ozono 4
char buf[20];
#define Sec " Sec."
#define Min " Min."

// Timer
uint16_t tempo_base = 1000;       // tempo confronto di 1 secondo
unsigned long int start_time;
unsigned long int end_time;
uint8_t secondo = 0;        // per conteggiare i secondi
uint8_t timer_ventola = 0; // durata pre/post ventilazione
uint8_t timer_ozono = 0;   // durata ozonizzazione

// inizializzazione del sistema
void setup (){
  oled.init(0x3C); // Inizializzazione display
  oled.startScreen();
// inizializzo pin I/O
  pinMode(ozono,OUTPUT);digitalWrite(ozono,LOW);
  pinMode(ventola,OUTPUT);digitalWrite(ventola,LOW);
  pinMode(start_stop,INPUT_PULLUP);
//  attivazione del timer  
  start_time = millis();
}

void loop (){
  gest_timer();
  gest_tasto();
  switch (stato){
    case 0:
      
    break;
    case 1:
      digitalWrite(ventola,HIGH);
      if(timer_ventola == 0){
        timer_ventola = durata_ventola;
        secondo = 0;
      }
    break;
    case 2:
      digitalWrite(ozono,HIGH);digitalWrite(ventola,HIGH);
      if(timer_ozono == 0){
        timer_ozono = durata_ozono;
        secondo = 0;
      }
    break;
    case 3:
      digitalWrite(ventola,HIGH);
      if(timer_ventola == 0){
        timer_ventola = durata_ventola * 2;
        secondo = 0;
      }
    break;
    case 4:
      digitalWrite(ozono,LOW);digitalWrite(ventola,LOW);
      timer_ozono = 0;timer_ventola = 0;
      stato = 0;
    break;
  }
}

// gestione timer
void gest_timer(){
  end_time = millis();
  if((end_time-start_time) >= tempo_base){
    start_time = end_time;
    if(timer_ventola != 0){
      timer_ventola-=1;
      if(timer_ventola == 0){
        digitalWrite(ventola,LOW);stato+=1;
      }
    }
    gest_visual();
    secondo+=1;
    if (secondo >= 60 ){
      secondo = 0;
      if(timer_ozono != 0){
        timer_ozono-=1;
        if(timer_ozono == 0){
          digitalWrite(ozono,LOW);stato+=1;
        }
      }
    }
  }
}

// gestione del pulsante
void gest_tasto(){
  if (digitalRead(start_stop) == LOW ){
    delay (200);
    while(digitalRead(start_stop) == LOW){}
    if(stato == 0){
      stato = 1;
    }else{
      stato = 4;
    }
  }
}

// visualizzazione dei dati su display Oled
void gest_visual(){
  // azzeramento area visibile del display
  oled.clear();oled.cursorTo(0, 0);
  
  switch (stato){
    case 0:
      oled.printString("Stato Riposo");
      oled.cursorTo(0, 18);
      oled.printString("Ozono OFF");
      oled.cursorTo(0, 28);
      oled.printString("Ventola OFF");
    break;
    case 1:
      oled.printString("Stato Preparazione");
      oled.cursorTo(0, 18);
      oled.printString("Ozono OFF");
      oled.cursorTo(0, 28);
      oled.printString("Ventola ON");
      oled.cursorTo(0, 38);
      oled.printString("Secondi ");
      sprintf(buf, "%0.2hd" Sec , timer_ventola);
      oled.printString(buf);
    break;
    case 2:
      oled.printString("Stato Ozonizzo");
      oled.cursorTo(0, 18);
      oled.printString("Ozono ON");
      oled.cursorTo(0, 28);
      oled.printString("Ventola ON");
      oled.cursorTo(0, 38);
      oled.printString("Minuti ");
      sprintf(buf, "%0.2hd" Min , timer_ozono);
      oled.printString(buf);
    break;
    case 3:
      oled.printString("Stato Diffusione");
      oled.cursorTo(0, 18);
      oled.printString("Ozono OFF");
      oled.cursorTo(0, 28);
      oled.printString("Ventola ON");
      oled.cursorTo(0, 38);
      oled.printString("Secondi ");
      sprintf(buf, "%0.2hd" Sec , timer_ventola);
      oled.printString(buf);
    break;
  }
}
