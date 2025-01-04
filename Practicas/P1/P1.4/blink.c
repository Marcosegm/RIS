#include "contiki.h"
#include "dev/leds.h"  /* Para el control de LEDs */
#include <stdio.h>

/*---------------------------------------------------------------------------*/
PROCESS(parpadeo_1_process, "Parpadeo LED 2");
PROCESS(parpadeo_2_process, "Parpadeo LED 3");
PROCESS(timer_process, "Proceso Timer");
AUTOSTART_PROCESSES(&timer_process, &parpadeo_1_process, &parpadeo_2_process);
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Proceso parpadeo_1_process: Conmutacion del LED 2 cada 2 segundos */
PROCESS_THREAD(parpadeo_1_process, ev, data)
{
  static struct etimer timer;

  PROCESS_BEGIN();

  /* Espera inicial hasta recibir un evento de inicio */
  PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_POLL);
  printf("Inicio parpadeo_1_process\n");

  /* Bucle para la conmutación del LED 2 cada 2 segundos */
  while (1) {
    etimer_set(&timer, CLOCK_SECOND * 2);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    leds_toggle(LEDS_LED2); /* Cambia LED 2 */
    printf("LED 2 conmutado\n");

    /* clock_time_t remaining_time = etimer_expiration_time(&timer) - clock_time(); 
    printf("Tiempo restante para el siguiente parpadeo de LED 2: %lu ticks\n", (unsigned long)remaining_time); */ 
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
/* Proceso parpadeo_2_process: Conmutacion del LED 3 cada 3 segundos */
PROCESS_THREAD(parpadeo_2_process, ev, data)
{
  static struct etimer timer;

  PROCESS_BEGIN();

  /* Espera inicial hasta recibir un evento de inicio */
  PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_POLL);
  printf("Inicio parpadeo_2_process\n");

  /* Bucle de conmutación cada 3 segundos */
  while (1) {
    etimer_set(&timer, CLOCK_SECOND * 3);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    leds_toggle(LEDS_LED3); /* Cambia LED 3 */
    printf("LED 3 conmutado\n");


    /* Imprimir el tiempo restante del temporizador 
    clock_time_t remaining_time = etimer_expiration_time(&timer) - clock_time();
    printf("Tiempo restante para el siguiente parpadeo de LED 3: %lu ticks\n", (unsigned long)remaining_time); */ 
    
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
/* Proceso timer_process: Envía eventos a los procesos que se encargan de los parpadeos de los led */
PROCESS_THREAD(timer_process, ev, data)
{
  static struct etimer timer;

  PROCESS_BEGIN();

  /* Espera 5 segundos antes de enviar eventos */
  etimer_set(&timer, CLOCK_SECOND * 5);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

  /* Envía eventos de inicio a los procesos de parpadeo */
  printf("Enviando eventos de inicio\n");
  process_poll(&parpadeo_1_process);
  process_poll(&parpadeo_2_process);
  

  
  PROCESS_END();
}