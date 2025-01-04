import serial
import time

def main():
    

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
        
        print("Serial Port /dev/ACM0 is opened")
    except IOError:
       
        print("serial port is already opened or does not exist")
        sys.exit(0)
    

    while True:
         # Reading from serial port  --> leyendo la informacion del puerto serial es decir, de la placa nrf52840
        line = ser.readline()
        # Print data received
    
        print("Serial Data: {0:s}".format(str(line, 'ascii').rstrip()))
        # Get data
        csv_fields=line.rstrip()
        fields=csv_fields.split(b'\x3B')
        print(fields)
        
        # debug
        index=0
        for value in fields:
            print("Field[{0:d}]: {1:f}".format(index, float(value)))
            index = index + 1
            
            
main()