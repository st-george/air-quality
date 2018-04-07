from pprint import pprint
import time
import serial
import re
import requests
import glob
from collections import defaultdict

serials = []

api_key = None

with open("thingspeak-api-key.txt", "r") as f:
  for line in f:
    api_key = line.rstrip()

for dev in glob.glob("/dev/tty.usbmodem*") + glob.glob("/dev/tty.wchusbserial*"):
  try:
    ser = serial.Serial(dev, 115200)
    serials.append(ser)
  except Exception, e:
    print e
    pass

last_send = 0

while True:
    data = defaultdict(dict)
    for ser in serials:
        l = ser.readline()
        print l

        # CSS811/HDC1080: T=28.77C, RH=24.29%, CO2=444, tVOC=6, ms=480731
        m = re.search('^CSS811\/HDC1080: T=([\d\.]+)C, RH=([\d\.]+)%, CO2=([\d\.]+), tVOC=([\d\.]+)', l)
        if m:
            data['hdc1080']['Temp'] = m.group(1)
            data['hdc1080']['Humidity'] = m.group(2)
            data['ccs811']['CO2'] = m.group(3)
            data['ccs811']['tVOC'] = m.group(4)

        # iAQcore: eco2=1858 ppm, tvoc=513 ppb, resist=279846 ohm, ms=6011
        m = re.search('^iAQcore: eco2=([\d\.]+) ppm, tvoc=([\d\.]+) ppb, resist=([\d\.]+) ohm', l)
        if m:
            data['iaqcore']['ECO2'] = m.group(1)
            data['iaqcore']['tVOC'] = m.group(2)
            data['iaqcore']['Resistance'] = m.group(3)

    if len(data) == 3:
        pprint(data)
        if time.time() - last_send > 20:
            print "Sending info to thingspeak"
            payload = {
                'key': api_key,
                'field1': data['hdc1080']['Temp'],
                'field2': data['hdc1080']['Humidity'],
                'field3': data['ccs811']['CO2'],
                'field4': data['ccs811']['tVOC'],
                'field5': data['iaqcore']['ECO2'],
                'field6': data['iaqcore']['tVOC'],
                'field7': data['iaqcore']['Resistance'] 
            }
            r = requests.post("https://api.thingspeak.com/update", params=payload)
            print r.text
            last_send = time.time()
    #else:
    #    print "SOMETHING IS WRONG"

    time.sleep(.1)
