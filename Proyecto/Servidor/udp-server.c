#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include <stdlib.h>
#include "sys/log.h"
#include "dev/radio.h"
#include "dev/leds.h"

#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

/* Variables para PANID y canal, que serán configuradas dinámicamente */
static uint16_t pan_id = 0xabcd;  // Valor por defecto
static uint8_t channel = 26;      // Valor por defecto

// Variables globales para almacenar los datos recibidos
static int switch_state ; // Estado del interruptor
static int16_t temperature = 0; // Temperatura
static int variable = 0;

// Variables globales para almacenar direcciones de los clientes
typedef struct {
  uip_ipaddr_t addr; // Dirección IPv6 del cliente
  int is_set;        // Indica si la dirección está configurada
} client_info_t;

// Variables globales para almacenar información de los clientes
static client_info_t client1_info = {.is_set = 0};
static client_info_t client2_info = {.is_set = 0};


static struct simple_udp_connection udp_conn;  // Maneja la conexion UDP

/* Función para configurar el PANID y el canal */
void configure_radio(uint16_t new_pan_id, uint8_t new_channel) {
  pan_id = new_pan_id;
  channel = new_channel;
  NETSTACK_RADIO.set_value(RADIO_PARAM_PAN_ID, pan_id);
  NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, channel);
}

PROCESS(udp_server_process, "UDP server");
AUTOSTART_PROCESSES(&udp_server_process);

/*---------------------------------------------------------------------------*/
static void udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  //LOG_INFO("Dato recibido: '%.*s'\n", datalen, data);  // Imprime el dato recibido como cadena

  if (strncmp((char *)data, "SWITCH:", 7) == 0) {
    // Mensaje del Cliente 1 (Estado del interruptor)
    if (!client1_info.is_set) {
      // Si aún no se ha registrado la dirección del Cliente 1
      uip_ipaddr_copy(&client1_info.addr, sender_addr);
      client1_info.is_set = 1;
      //LOG_INFO("Dirección IPv6 del Cliente 1 registrada.\n");
    }
    switch_state = atoi((char *)data + 7);  // Extrae el estado del interruptor
    //LOG_INFO("Estado del interruptor recibido: %d\n", switch_state);
    // Enviar el estado del interruptor al Cliente 2
    if (client2_info.is_set) { // Verifica que la dirección del Cliente 2 esté configurada
      char msg[32];
      snprintf(msg, sizeof(msg), "SWITCH_STATE:%d", switch_state); // Formato del mensaje
      simple_udp_sendto(&udp_conn, msg, strlen(msg), &client2_info.addr); // Envía el mensaje
      //LOG_INFO("Estado del interruptor enviado al Cliente 2: %s\n", msg);
    } else {
      //LOG_INFO("Dirección del Cliente 2 no configurada. No se puede enviar el dato.\n");
    }
  } 
  else if (strncmp((char *)data, "TEMP:", 5) == 0) {
    // Mensaje del Cliente 2 (Temperatura)
    if (!client2_info.is_set) {
      // Si aún no se ha registrado la dirección del Cliente 2
      uip_ipaddr_copy(&client2_info.addr, sender_addr);
      client2_info.is_set = 1;
      //LOG_INFO("Dirección IPv6 del Cliente 2 registrada.\n");
    }
    char temp_str[16]; // Un buffer temporal para extraer el valor
    strncpy(temp_str, (char *)data + 5, datalen - 5); // Copiar solo la parte después de "TEMP:"
    temp_str[datalen - 5] = '\0'; // Asegúrate de que el string esté terminado
    temperature = atoi(temp_str); // Convertir a entero
    //LOG_INFO("Temperatura recibida: %d\n", temperature);
  }
  if (client1_info.is_set && client2_info.is_set) {
      leds_on(LEDS_GREEN);
  } else {
      leds_on(LEDS_RED);
    //LOG_INFO("Direcciones de los clientes configuradas. Listo para enviar datos.\n");

  }
  if (switch_state == 0){
    printf("%d;%d;%d\n",temperature, variable,  switch_state );
  } else {
    printf("%d;%d;%d\n",variable, temperature,  switch_state );
  }
  
}
 
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
  PROCESS_BEGIN();

/* Configuración del PANID y canal */
  configure_radio(0x5678, 23);  // Configura el PANID y canal 


  /* Initialize DAG root */
  NETSTACK_ROUTING.root_start();

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_SERVER_PORT, NULL,
                      UDP_CLIENT_PORT, udp_rx_callback);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
