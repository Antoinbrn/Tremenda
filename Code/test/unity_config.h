#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

#define UNITY_INCLUDE_CONFIG_H
#define UNITY_INT_WIDTH 32
#define UNITY_LONG_WIDTH 32
#define UNITY_POINTER_WIDTH 32

#define UNITY_OUTPUT_CHAR(char) Serial.print(char)
#define UNITY_OUTPUT_FLUSH() Serial.flush()
#define UNITY_OUTPUT_START() do { Serial.begin(115200); delay(100); } while(0)
#define UNITY_OUTPUT_COMPLETE() do { } while(0)

#define UNITY_FIXTURES 1
#define UNITY_TEST_SUMMARY 1

#endif