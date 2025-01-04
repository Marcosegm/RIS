#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "sys/log.h"
#include "dev/radio.h"
#include <stdlib.h>

// Definición de los puertos UDP
#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

static struct simple_udp_connection udp_conn;

PROCESS(udp_server_process, "UDP server");
AUTOSTART_PROCESSES(&udp_server_process);

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
  int celsius = atoi((char *)data);
  int fahrenheit = 2 * celsius + 32;
  char response[32];


  snprintf(response, sizeof(response), "%d", fahrenheit);

  LOG_INFO("Informacion recibida:'%d' Celsius del nodo --> ", celsius);
  LOG_INFO_6ADDR(sender_addr);
  LOG_INFO_("\n");
  LOG_INFO("TEMPERATURA RECIBIDA = %d",celsius);
  LOG_INFO_("\n");
  LOG_INFO("PAN ID configurado: 0x%04X\n", pan_id);
  LOG_INFO("CANAL configurado: %d\n", channel);
  
#if WITH_SERVER_REPLY
  /* send back the converted temperature to the client */
  LOG_INFO("Enviando temperatura en Fahrenheit = '%d'\n", fahrenheit);
  LOG_INFO("Destino = ");
  LOG_INFO_6ADDR(sender_addr);
  LOG_INFO_("\n");
  simple_udp_sendto(&udp_conn, response, strlen(response), sender_addr);
#endif /* WITH_SERVER_REPLY */
}

PROCESS_THREAD(udp_server_process, ev, data)
{
  PROCESS_BEGIN();

  /* Configuración del PANID y canal */
  configure_radio(0xacbd, 16);  // Configura el PANID y canal 

  /* Inicializar DAG root */
  NETSTACK_ROUTING.root_start();

  /* Inicializar la conexión UDP */
  simple_udp_register(&udp_conn, UDP_SERVER_PORT, NULL, UDP_CLIENT_PORT, udp_rx_callback);

  PROCESS_END();
}
