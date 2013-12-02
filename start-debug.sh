#!/bin/bash

qemu-system-arm -kernel kernel.img -cpu arm1176 -M raspi -m 512 -s -vnc :1
