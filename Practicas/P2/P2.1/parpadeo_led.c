#include "contiki.h"
#include "dev/leds.h"  /* Para el control de LEDs */
#include <stdio.h>
#include "sys/etimer.h"

/*---------------------------------------------------------------------------*/
PROCESS(tarea_1_process, "Parpadeo LED GREEN");
PROCESS(tarea_2_process, "Parpadeo LED YELLOW");
PROCESS(tarea_3_process, "Proceso Timer");
AUTOSTART_PROCESSES(&tarea_3_process, &tarea_1_process, &tarea_2_process);
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Proceso tarea_1_process: Conmutacion del LED GREEN cada 2 segundos */
PROCESS_THREAD(tarea_1_process, ev, data)
{
  static struct etimer timer;

  PROCESS_BEGIN();

  /* Espera inicial hasta recibir un evento de inicio */
  PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_POLL);
  printf("Inicio tarea_1_process\n");

  /* Bucle para la conmutación del LED Green cada 2 segundos */
  while (1) {
    etimer_set(&timer, CLOCK_SECOND * 2);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    leds_toggle(LEDS_GREEN); /* Cambia LED GREEN */
    printf("LED GREEN conmutado\n"); 
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
/* Proceso Tarea_2_process: Conmutacion del LED YELLOW cada 4 segundos */
PROCESS_THREAD(tarea_2_process, ev, data)
{
  static struct etimer timer;

  PROCESS_BEGIN();

  /* Espera inicial hasta recibir un evento de inicio */
  PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_POLL);
  printf("Inicio tarea_2_process\n");

  /* Bucle de conmutación cada 3 segundos */
  while (1) {
    etimer_set(&timer, CLOCK_SECOND * 4);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    leds_toggle(LEDS_YELLOW); /* Cambia LED YELLOW */
    printf("LED YELLOW conmutado\n");
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
/* Proceso tarea_3_process: Envía eventos a los procesos que se encargan de los parpadeos de los led */
PROCESS_THREAD(tarea_3_process, ev, data)
{
  static struct etimer timer;

  PROCESS_BEGIN();

  /* Espera 3 segundos antes de enviar eventos */
  etimer_set(&timer, CLOCK_SECOND * 3);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

  /* Envía eventos de inicio a los procesos de parpadeo */
  printf("Enviando eventos de inicio\n");
  process_poll(&tarea_1_process);
  process_poll(&tarea_2_process);
  
  PROCESS_END();
}