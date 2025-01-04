#include "contiki.h"
#include <stdio.h> 
#include "dev/leds.h"
#include "lib/sensors.h"
#include "common/temperature-sensor.h"
#include "dev/button-hal.h"

#define BUTTON_ID   BUTTON_HAL_ID_BUTTON_ZERO // Identificador único del botón

// Variables globales
static int16_t raw_tmp = 0; // Temperatura leída del sensor
static int16_t int_temp_c = 0; // Parte entera de la temperatura en Celsius
static int16_t frac_temp_c = 0; // Parte fraccionaria de la temperatura en Celsius
static int16_t int_temp_f = 0; // Parte entera de la temperatura en Fahrenheit
static int16_t frac_temp_f = 0;  // Parte fraccionaria de la temperatura en Fahrenheit
static int switch_state = 0;    // Estado del interruptor (0 o 1)
 
/*---------------------------------------------------------------------------*/
PROCESS(timer_process, "timer_process");
PROCESS(temp_process, "temp_process");
PROCESS(switch_process, "switch_process");
AUTOSTART_PROCESSES(&timer_process, &temp_process, &switch_process);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(timer_process, ev, data) {
  static struct etimer timer;

  PROCESS_BEGIN();

  while (1) {
    etimer_set(&timer, CLOCK_SECOND * 2);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    etimer_reset(&timer);
    process_poll(&temp_process);
    process_poll(&switch_process);
    printf("%d.%d;%d.%d;%d\n", int_temp_c, frac_temp_c, int_temp_f, frac_temp_f, switch_state);
  }

  PROCESS_END();
}

PROCESS_THREAD(temp_process, ev, data) {
  PROCESS_BEGIN();

  while (1) {
    PROCESS_WAIT_EVENT();
    SENSORS_ACTIVATE(temperature_sensor);

    raw_tmp = (int16_t)temperature_sensor.value(0);
    // Grados Celsius
    int_temp_c = raw_tmp >> 2;
    frac_temp_c = (raw_tmp & 0x3) * 25;
    // Conversión a Fahrenheit
    int_temp_f = (int_temp_c * 2) + 32;
    frac_temp_f = (frac_temp_c * 2) % 100;
  }

  PROCESS_END();
}

PROCESS_THREAD(switch_process, ev, data) {
  PROCESS_BEGIN();
  button_hal_init();

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == button_hal_periodic_event);
    button_hal_button_t *btn = (button_hal_button_t *)data;
    if (btn->unique_id == BUTTON_ID) {
      switch_state = !switch_state;
      if (switch_state) {
        leds_on(LEDS_GREEN);
        leds_off(LEDS_RED);
      } else {
        leds_off(LEDS_GREEN);
        leds_on(LEDS_RED);
      }
    }
  }

  PROCESS_END();
}
