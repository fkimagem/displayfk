#ifndef DEBUG_H
#define DEBUG_H

#define DEBUG_D(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
#define DEBUG_E(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
#define DEBUG_W(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
#define DEBUG_I(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
#define DEBUG_V(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)

#endif
