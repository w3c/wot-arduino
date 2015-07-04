// scaffolding in lieu of the genuine Arduino.h

#ifndef ARDUINO
#define ARDUINO

#include <string>
#include <iostream>
using namespace std;

#define null 0
#define false 0
#define true  (!false)
typedef bool boolean;
#define max(a,b) ((a)>(b)?(a):(b))

// these are transformed by sed script into Serial.print, etc.

#define PRINT(x)   (cout << x)
#define PRINTLN(x)  (cout << x << endl)

#endif

