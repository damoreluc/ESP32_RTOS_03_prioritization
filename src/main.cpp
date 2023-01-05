/*
 * FreeRTOS Esempio 03: task prioritization
 *
 * Due task a differente priorità con stampa concorrente su console
 * 
 * Nota: nel file soc.h sono definiti i riferimenti ai due core della ESP32:
 *   #define PRO_CPU_NUM (0)
 *   #define APP_CPU_NUM (1)
 * 
 * Qui viene adoperata la APP_CPU
 * 
*/

#include <Arduino.h>

// messaggio da stampare sulla console tramite il primo task
const char messaggio[] = "FreeRTOS sistema operativo real time";

// handler dei due task
static TaskHandle_t task1 = NULL;
static TaskHandle_t task2 = NULL;

// Implementazione dei due task
// task con priorità bassa per stampa del messaggio
void fnTask1(void *parameters) {

  uint16_t msgLen = strlen(messaggio);

  // stampa sul terminale
  while(1) {
    Serial.println();
    for(uint16_t i=0; i<msgLen; i++) {
      Serial.print(messaggio[i]);
    }
    Serial.println();

    vTaskDelay(1000 / portTICK_PERIOD_MS);  
  }
}

// task con priorità alta per stampa * su terminale
void fnTask2(void *parameters) {

  // stampa sul terminale
  while(1) {
    Serial.print('*');

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// configurazione del sistema
void setup() {
  // velocità molto bassa per vedere l'effetto della preemption dei due task
  Serial.begin(300);

  // breve pausa
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println("FreeRTOS: task con diversa priorità");

  Serial.print("I task setup e loop girano sul core ");
  Serial.println( xPortGetCoreID() );
  Serial.print("con priorità ");
  Serial.println( uxTaskPriorityGet(NULL) );


  // creazione ed avvio del task a bassa priorità
  xTaskCreatePinnedToCore(
    fnTask1,      // funzione da richiamare nel task
    "Task1",      // nome del task (etichetta utile per debug)
    1024,         // dimensione in byte dello stack per le variabili locali del task (minimo 768 byte)
    NULL,         // puntatore agli eventuali parametri da passare al task
    1,            // priorità del task
    &task1,       // eventuale task handle per gestire il task da un altro task
    APP_CPU_NUM   // core su cui far girare il task
  );

  // creazione ed avvio del task a bassa priorità
  xTaskCreatePinnedToCore(
    fnTask2,      // funzione da richiamare nel task
    "Task2",      // nome del task (etichetta utile per debug)
    1024,         // dimensione in byte dello stack per le variabili locali del task (minimo 768 byte)
    NULL,         // puntatore agli eventuali parametri da passare al task
    2,            // priorità del task
    &task2,       // eventuale task handle per gestire il task da un altro task
    APP_CPU_NUM   // core su cui far girare il task
  );
}

void loop() {
  // sospende il task ad alta priorità per un certo numero di volte
  for(int i = 0; i<3; i++) {
    vTaskSuspend(task2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    vTaskResume(task2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }

  // termina il task1 (quello a bassa priorità)
  if(task1 != NULL) {
    vTaskDelete(task1);
    task1 = NULL;
  }

}