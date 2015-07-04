#!/bin/sh
cp ~/Projects/wot-arduino/*.cpp .
cp ~/Projects/wot-arduino/*.h .
rm demo.cpp arduino.h
for i in *.cpp; do
  sed -i "" s/PRINTLN/Serial.println/g $i
  sed -i "" s/PRINT/Serial.print/g $i
done

