#include "contiki.h"
#include "net/routing/routing.h"
#include "random.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "sys/log.h"
#include <stdlib.h>
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

#define SEND_INTERVAL		  (60 * CLOCK_SECOND)

static struct simple_udp_connection udp_conn;

/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
AUTOSTART_PROCESSES(&udp_client_process);
/*---------------------------------------------------------------------------*/

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


static void udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  LOG_INFO("Temperatura en Farhrenheit '%.*s' grados calculada por el servidor --> ", datalen, (char *) data);
  LOG_INFO_6ADDR(sender_addr);
#if LLSEC802154_CONF_ENABLED
  LOG_INFO_(" LLSEC LV:%d", uipbuf_get_attr(UIPBUF_ATTR_LLSEC_LEVEL));
#endif
  LOG_INFO_("\n");
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data)
{
  static struct etimer periodic_timer;
  static unsigned count;
  static char str[32];
  uip_ipaddr_t dest_ipaddr;

  PROCESS_BEGIN();

  /* Configuración del PANID y canal */
  configure_radio(0xabcd, 16);  // Configura el PANID y canal 

  /* Inicializar la conexión UDP */
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,
                      UDP_SERVER_PORT, udp_rx_callback);

  etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);
  
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

    if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {
      /* Enviar al root del DAG */
      int temperature;
      if (count % 2 == 0) {
        temperature = 38;
        LOG_INFO("Enviando temperatura interna en Celsius = %d\n", temperature);
      } else {
          temperature = 35;
          LOG_INFO("Enviando temperatura externa en Celsius = %d\n", temperature);
      }
      snprintf(str, sizeof(str), "%d", temperature);
      simple_udp_sendto(&udp_conn, str, strlen(str), &dest_ipaddr);
      count++;
      LOG_INFO("PAN ID configurado: 0x%04X\n", pan_id);
      LOG_INFO("CANAL configurado: %d\n", channel);
    } else {
      LOG_INFO("No Accesible\n");
    }

    /* Añadir algo de jitter */
    etimer_set(&periodic_timer, SEND_INTERVAL
      - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
