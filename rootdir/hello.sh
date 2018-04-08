#!/system/bin/sh

mount -o rw,remount /system

date +%s%N > /system/hello.txt

dmesg --follow | tee /system/kmsg.txt