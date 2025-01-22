#include "contiki.h"
#include "net/routing/routing.h"
#include "random.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "sys/log.h"
#include "lib/sensors.h"
#include "common/temperature-sensor.h"
#include "dev/radio.h"
#include <stdlib.h>

#define BUTTON_ID   BUTTON_HAL_ID_BUTTON_ZERO // Identificador único del botón

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

#define SEND_INTERVAL		  (5 * CLOCK_SECOND)

static struct simple_udp_connection udp_conn; // Maneja la conexion UDP

// Variables globales
static int16_t raw_tmp = 0; // Temperatura leída del sensor
static int16_t temp_c = 0; // Parte entera de la temperatura en Celsius
static int16_t temp_f = 0; // Temperatura en Fahrenheit
static int received_switch_state = 0; // Estado del interruptor recibido del servidor
/* Variables para PANID y canal, que serán configuradas dinámicamente */

static uint16_t pan_id = 0xabcd;  // Valor por defecto
static uint8_t channel = 26;      // Valor por defecto


/* Función para configurar el PANID y el canal */
void configure_radio(uint16_t new_pan_id, uint8_t new_channel) {
  pan_id = new_pan_id;
  channel = new_channel;
  NETSTACK_RADIO.set_value(RADIO_PARAM_PAN_ID, pan_id);
  NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, channel);
}

/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
AUTOSTART_PROCESSES(&udp_client_process);
/*---------------------------------------------------------------------------*/


static void udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{

  LOG_INFO("Mensaje recibido en desde el servidor: '%.*s'\n", datalen, data);

  if (strncmp((char *)data, "SWITCH_STATE:", 13) == 0) {
    received_switch_state = atoi((char *)data + 13);  // Extrae el estado del interruptor
    LOG_INFO("Estado del interruptor recibido del servidor: %d\n", received_switch_state);
    // Realiza las acciones necesarias en función del estado del interruptor
  }

#if LLSEC802154_CONF_ENABLED
  LOG_INFO_(" LLSEC LV:%d", uipbuf_get_attr(UIPBUF_ATTR_LLSEC_LEVEL));
#endif
  LOG_INFO_("\n");

}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data)
{

  static struct etimer periodic_timer;
  static char str[32];
  uip_ipaddr_t dest_ipaddr;

  PROCESS_BEGIN();

  /* Configuración del PANID y canal */
  configure_radio(0x5678, 23); // Configura el PANID y canal 


  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,
                      UDP_SERVER_PORT, udp_rx_callback);

  etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

    SENSORS_ACTIVATE(temperature_sensor);
    raw_tmp = (int16_t)temperature_sensor.value(0);
    temp_c = raw_tmp >> 2;

    if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {
       
      //snprintf(str, sizeof(str), "Temperatura %d", temp_c);
      memset(str, 0, sizeof(str)); // Limpia el buffer
      if (received_switch_state == 0 ) {
        snprintf(str, sizeof(str), "TEMP:%d", temp_c);
        LOG_INFO("Enviando: %s\n", str);  // Imprime el dato antes de enviarlo
        simple_udp_sendto(&udp_conn, str, strlen(str), &dest_ipaddr);
        //LOG_INFO("Enviando Temperatura %d\n", temp_c);
      } else {
        temp_f = (temp_c * 2) + 32;
        snprintf(str, sizeof(str), "TEMP:%d", temp_f);
        LOG_INFO("Enviando: %s\n", str);  // Imprime el dato antes de enviarlo
        simple_udp_sendto(&udp_conn, str, strlen(str), &dest_ipaddr);
        //LOG_INFO("Enviando Temperatura %d\n", temp_f);
      }
      
      
    } else {
      LOG_INFO("Not reachable yet\n");
    }

    /* Add some jitter */
    etimer_set(&periodic_timer, SEND_INTERVAL
      - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
