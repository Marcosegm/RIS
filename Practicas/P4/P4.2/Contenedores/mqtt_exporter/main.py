#!/usr/bin/env python3

""" MQTT exporter """

import json
#from msilib.schema import ServiceInstall
import re
import signal
import logging
import os
import sys
import serial

import paho.mqtt.client as mqtt
from prometheus_client import Counter, Gauge, start_http_server

logging.basicConfig(filename='register.log', level=logging.DEBUG)
LOG = logging.getLogger("[mqtt-exporter]")

########################################################################
# Prometheus
########################################################################

prom_msg_counter = None
prom_temp_c_gauge = None
prom_temp_F_gauge = None
prom_switch__state_gauge = None

def create_msg_counter_metrics():
    global prom_msg_counter

    prom_msg_counter = Counter( 'number_msgs',
        'Number of received messages'
    )

def create_global_gauge_metrics():
    global prom_temp_c_gauge
    global prom_temp_F_gauge
    global prom_switch__state_gauge

    prom_temp_c_gauge = Gauge( 'temp_c',
        'Temperature [Celsius Degrees]'
    )
    prom_temp_F_gauge = Gauge( 'temp_F',
        'Temperature [Fahrenheit Degrees]'
    )
    prom_switch__state_gauge = Gauge( 'switch_state',
        'Switch state'
    )

def parse_message(raw_topic, raw_payload):
    try:
        payload = json.loads(raw_payload)
    except json.JSONDecodeError:
        LOG.error(" Failed to parse payload as JSON: %s", str(payload, 'ascii'))
        print(" Failed to parse payload as JSON: {0:s}".format(str(payload, 'ascii')))
        return None, None
    except UnicodeDecodeError:
        LOG.error(" Encountered undecodable payload: %s", raw_payload)
        print(" Encountered undecodable payload: {0:s}".format(raw_payload))
        return None, None
    
    topic = raw_topic

    return topic, payload

def parse_metric(data):
    if isinstance(data, (int,float)):
        return data
    
    if isinstance(data, bytes):
        data = data.decode()

    if isinstance(data, str):
        data = data.upper()

    return float(data)

def parse_metrics(data, topic, client_id):
    for metric, value in data.items():
        if isinstance(value, dict):
            LOG.debug(" Parsing dict %s, %s", metric, value)
            print(" Parsing dict {0:s}, {1:s}".format(metric, value))
            parse_metrics(value, topic, client_id)
            continue

        try:
            metric_value = parse_metric(value)

        except ValueError as err:
            LOG.error(" Failed to convert %s, Error: %s", metric, err)
            print(" Failed to convert {0:s}, Error: {1:s}".format(metric, err))

########################################################################
# MQTT
########################################################################
def on_connect(client, _, __, rc):
    LOG.info(" Connected with result code: %s", rc)
    print(" Connected with result code: {0:d}".format(rc))
    
    client.subscribe("temp_c")
    client.subscribe("temp_F")
    client.subscribe("switch_state")
    
    if rc != mqtt.CONNACK_ACCEPTED:
        LOG.error("[ERROR]: MQTT %s", rc)
        print("[ERROR]: MQTT {0:d}".format(rc))

def on_message(_, userdata, msg):
    LOG.info(" [Topic: %s] %s", msg.topic, msg.payload)
    print(" [Topic: {0:s}] {1:s}".format(str(msg.topic), str(msg.payload)))

    topic, payload = parse_message(msg.topic, msg.payload)
    LOG.debug(" \t Topic: %s", topic)
    print(" \t Topic: {0:s}".format(str(topic)))

    LOG.debug(" \t Payload: %s", payload)
    print(" \t Payload: {0:s}".format(str(payload)))

    # if not topic or not payload:
    #     LOG.error(" [ERROR]: Topic or Payload not found")
    #     print(" [ERROR]: Topic or Payload not found")

    #     return

    prom_msg_counter.inc()
    if(msg.topic == "temp_c"):
        prom_temp_c_gauge.set(payload)
    if(msg.topic == "temp_F"):
        prom_temp_F_gauge.set(payload)
    if(msg.topic == "switch_state"):
        prom_switch__state_gauge.set(payload)

########################################################################
# Main
########################################################################
def main():
    # Create MQTT client
    client = mqtt.Client()

    def stop_reques(signum, frame):
        LOG.debug(" Stopping MQTT exporter")
        print(" Stopping MQTT exporter")

        client.disconnect()
        ser.close()
        sys.exit(0)

    #Serial port --> aqui obtiene los datos del codigo compilado en la placa nrf52840 
    try:
        ser = serial.Serial(port="/dev/ttyACM0",
                            baudrate=115200,
                            parity=serial.PARITY_NONE,
                            stopbits=serial.STOPBITS_ONE,
                            bytesize=serial.EIGHTBITS,
                            timeout=2000)
        ser.flushInput()
        ser.flush()
        ser.isOpen()
        LOG.info("Serial Port /dev/ACM0 is opened")
        print("Serial Port /dev/ACM0 is opened")
    except IOError:
        LOG.error("serial port is already opened or does not exist")
        print("serial port is already opened or does not exist")
        sys.exit(0)

    # Create Prometheus metrics
    create_msg_counter_metrics()
    create_global_gauge_metrics()

    # Start prometheus server
    start_http_server(9000)

    # Configure MQTT topic
    client.on_connect = on_connect
    client.on_message = on_message
    # Suscribe MQTT topics
    LOG.debug(" Connecting to localhost")
    # Connect to MQTT broker
    client.connect("localhost", 1883, 60)
    # Waiting for messages
    client.loop_start()
    
    fields = []

    while True:
        # Reading from serial port  --> leyendo la informacion del puerto serial es decir, de la placa nrf52840
        line = ser.readline()
        
        # Print data received
        # LOG.debug("Serial Data: %s", str(line, 'ascii').rstrip())
        # print("Serial Data: {0:s}".format(str(line, 'ascii').rstrip()))
        
        # Get data from serial port
        csv_fields = line.rstrip()
        fields = csv_fields.split(b'\x3B')    
        
        
        #Validar que haya al menos 3 campos
        if len(fields) < 3:
            LOG.error(" [ERROR]: Invalid number of fields")
            print(" [ERROR]: Invalid number of fields")
            continue
        
        # debug
        index=0
        for value in fields:
            LOG.debug("Field[%d]: %f", index, float(value))
            print("Field[{0:d}]: {1:f}".format(index, float(value)))
            index = index + 1
        
        # Publish data on corresponding topic
        client.publish(topic="temp_c", payload=fields[0], qos=0, retain=False)
        client.publish(topic="temp_F", payload=fields[1], qos=0, retain=False)
        client.publish(topic="switch_state", payload=fields[2], qos=0, retain=False)


########################################################################
# Main
########################################################################
if __name__ == "__main__":
    main()