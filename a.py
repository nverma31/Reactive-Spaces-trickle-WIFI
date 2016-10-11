import serial
import time


ser = serial.Serial('/dev/cu.usbmodem1421', 9600)
time.sleep(1.5)
written = ser.write('6')
ser.close()
print "Bytes Written to port:", written
