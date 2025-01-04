#include "contiki.h"
#include <stdio.h> /* For printf() */
#include <string.h>
#include "dev/leds.h"
#include "lib/sensors.h"
#include "common/temperature-sensor.h"

/*---------------------------------------------------------------------------*/
PROCESS(timer_process, "timer_process");
PROCESS(temp_process, "temp_process");
AUTOSTART_PROCESSES(&timer_process, &temp_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(timer_process, ev, data){
  static struct etimer timer;

  PROCESS_BEGIN();
  
  while(1){
    //printf("Timer process\n");
    etimer_set(&timer, CLOCK_SECOND * 2);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    etimer_reset(&timer);
    process_poll(&temp_process);
  }

  PROCESS_END();
}

PROCESS_THREAD(temp_process, ev, data){
  static int16_t raw_tmp = 0;

  static int16_t int_tmp_c = 0;
  static int16_t frac_tmp_c = 0;

// Se define las variables para la conversion a grados farenheit
  static int16_t int_tmp_f = 0;
  static int16_t frac_tmp_f = 0;
  

  PROCESS_BEGIN();

  while(1){
    PROCESS_WAIT_EVENT();
    SENSORS_ACTIVATE(temperature_sensor);
    
    raw_tmp = (int16_t)temperature_sensor.value(0);
    
    // Integer Celsius Degrees for printf
    int_tmp_c = raw_tmp >> 2;
    frac_tmp_c = (raw_tmp & 0x3)*25;


    // conversion a grados farenheit
    int_tmp_f = (int_tmp_c * 2) + 32;
    frac_tmp_f = (frac_tmp_c * 2) / 100;

    printf("%d.%d\n", int_tmp_f, frac_tmp_f);
  }

  PROCESS_END();
}
