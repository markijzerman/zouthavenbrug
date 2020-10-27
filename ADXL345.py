import smbus
import time
from time import sleep
import sys

#libs for osc
import argparse
from pythonosc import udp_client

bus = smbus.SMBus(1)

bus.write_byte_data(0x53, 0x2C, 0x0B)
value = bus.read_byte_data(0x53, 0x31)
value &= ~0x0F;
value |= 0x0B;  
value |= 0x08;
bus.write_byte_data(0x53, 0x31, value)
bus.write_byte_data(0x53, 0x2D, 0x08)

#osc sender args
parser = argparse.ArgumentParser()
parser.add_argument("--ip", default="127.0.0.1")
parser.add_argument("--port", type=int, default=5005)
args = parser.parse_args()
client = udp_client.SimpleUDPClient(args.ip, args.port)


def getAxes():
    bytes = bus.read_i2c_block_data(0x53, 0x32, 6)
        
    x = bytes[0] | (bytes[1] << 8)
    if(x & (1 << 16 - 1)):
        x = x - (1<<16)

    y = bytes[2] | (bytes[3] << 8)
    if(y & (1 << 16 - 1)):
        y = y - (1<<16)

    z = bytes[4] | (bytes[5] << 8)
    if(z & (1 << 16 - 1)):
        z = z - (1<<16)

    x = x * 0.004 
    y = y * 0.004
    z = z * 0.004

    x = x * 9.80665
    y = y * 9.80665
    z = z * 9.80665

    x = round(x, 4)
    y = round(y, 4)
    z = round(z, 4)

    #print("   x = %.3f ms2" %x)
    #print("   y = %.3f ms2" %y)
    #print("   z = %.3f ms2" %z)
    client.send_message("/a", [x, y, z])


    
    return {"x": x, "y": y, "z": z}
    
try:
    while True: 
        getAxes()
        #client.send_message("/accel", [x, y, z])
        time.sleep(0.01)
except KeyboardInterrupt:
    sys.exit()
