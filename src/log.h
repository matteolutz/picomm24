#ifndef LOG_H
#define LOG_H

#define LOG_LEVEL_NONE 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_INFO 3
#define LOG_LEVEL_DEBUG 4

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_DEBUG
#endif

const char *__LOG_LEVEL_STRINGS[] = {
    "NONE",
    "ERROR",
    "WARNING",
    "INFO",
    "DEBUG"};

#define LOG(level, ...)                               \
    do                                                \
    {                                                 \
        if (level <= LOG_LEVEL)                       \
        {                                             \
            Serial.print("[picomm][");                \
            Serial.print(__LOG_LEVEL_STRINGS[level]); \
            Serial.print("][");                       \
            Serial.print(__FILE__);                   \
            Serial.print(":");                        \
            Serial.print(__LINE__);                   \
            Serial.print("] ");                       \
            Serial.printf(__VA_ARGS__);               \
            Serial.println();                         \
        }                                             \
    } while (0)

#define LOG_ERROR(...) LOG(LOG_LEVEL_ERROR, __VA_ARGS__)
#define LOG_WARNING(...) LOG(LOG_LEVEL_WARNING, __VA_ARGS__)
#define LOG_INFO(...) LOG(LOG_LEVEL_INFO, __VA_ARGS__)
#define LOG_DEBUG(...) LOG(LOG_LEVEL_DEBUG, __VA_ARGS__)

#define LOG_INIT()            \
    do                        \
    {                         \
        Serial.begin(115200); \
        while (!Serial)       \
            ;                 \
    } while (0)

#endif // LOG_H