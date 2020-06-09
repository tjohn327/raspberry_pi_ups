from powerpi import Powerpi
import os, sys

ppi = Powerpi()
if ppi.initialize():
    sys.exit(1)

print(ppi.read_status())
ppi.bat_disconnect()
os.system('sudo shutdown now')