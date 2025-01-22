#include "contiki.h"
#include "net/routing/routing.h"
#include "random.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "dev/button-hal.h"
#include "sys/log.h"
#include "dev/leds.h"
#include "dev/radio.h"

#define BUTTON_ID   BUTTON_HAL_ID_BUTTON_ZERO // Identificador único del botón

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

#define SEND_INTERVAL		  (10 * CLOCK_SECOND)


/* Variables para PANID y canal, que serán configuradas dinámicamente */

static uint16_t pan_id = 0xabcd;  // Valor por defecto
static uint8_t channel = 26;      // Valor por defecto

/*---------------------------------------------------------------------------*/

static int switch_state;    // Estado del interruptor (0 o 1)

/* Función para configurar el PANID y el canal */
void configure_radio(uint16_t new_pan_id, uint8_t new_channel) {
  pan_id = new_pan_id;
  channel = new_channel;
  NETSTACK_RADIO.set_value(RADIO_PARAM_PAN_ID, pan_id);
  NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, channel);
}

static struct simple_udp_connection udp_conn; // Maneja la conexion UDP

/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
PROCESS(switch_process, "switch_process");
AUTOSTART_PROCESSES(&udp_client_process, &switch_process);
/*---------------------------------------------------------------------------*/
static void
udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{

  //LOG_INFO("Received response '%.*s' from ", datalen, (char *) data);
  //LOG_INFO_6ADDR(sender_addr);

#if LLSEC802154_CONF_ENABLED
  LOG_INFO_(" LLSEC LV:%d", uipbuf_get_attr(UIPBUF_ATTR_LLSEC_LEVEL));
#endif
  LOG_INFO_("\n");

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

    if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {
       
      //snprintf(str, sizeof(str), "Estado Interruptor %d", switch_state);
      snprintf(str, sizeof(str), "SWITCH:%d", switch_state);
      LOG_INFO("Enviando: %s\n", str);  // Imprime el dato antes de enviarlo
      simple_udp_sendto(&udp_conn, str, strlen(str), &dest_ipaddr);
      LOG_INFO("Enviando Estado Interruptor %d\n", switch_state);
      
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
