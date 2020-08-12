#!/usr/bin/python

import os, sys
import logging
from powerpi import Powerpi

logging.basicConfig(level=logging.INFO)

if __name__=="__main__":
    ppi = Powerpi()
    if ppi.initialize():
        sys.exit(1)
    else:
        sys.exit(0)
        
