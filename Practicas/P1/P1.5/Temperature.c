#include "contiki.h"
#include <stdio.h> /* For printf() */
#include <string.h>
#include "dev/leds.h"
#include "lib/sensors.h"
#include "common/temperature-sensor.h"


/*---------------------------------------------------------------------------*/
PROCESS(temperature_process, "Temperature process");
PROCESS(periodic_process, "Periodic process");
AUTOSTART_PROCESSES(&temperature_process, &periodic_process);
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(temperature_process, ev, data)
{
  static int16_t raw_tmp = 0;
  static int16_t int_tmp_c = 0;
  static int16_t frac_tmp_c = 0;
  
  PROCESS_BEGIN();

  while(1){
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_POLL);

    SENSORS_ACTIVATE(temperature_sensor);
    
    raw_tmp = (int16_t)temperature_sensor.value(0);
    int_tmp_c = raw_tmp >> 2;
    frac_tmp_c = (raw_tmp & 0x3)*25;
    printf("%d.%d\n", int_tmp_c, frac_tmp_c);
  }
  
  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(periodic_process, ev, data)
{
  static struct etimer timer;

  PROCESS_BEGIN();

  etimer_set(&timer, CLOCK_SECOND * 3);

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    process_poll(&temperature_process);
    etimer_reset(&timer);
  }

  PROCESS_END();
}