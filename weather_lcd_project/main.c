#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "lcd1602.h"
#include "http_client.h"
#include "jsmn.h"

#define LAT "51.5074"
#define LON "-0.1278"
#define API_KEY "YOUR_API_KEY"
#define WEATHER_INTERVAL 1800

char weather_desc[32] = "Loading...";

int jsmn_eq(const char *json, jsmntok_t *tok, const char *s) {
    return (tok->type == JSMN_STRING &&
            (int)strlen(s) == tok->end - tok->start &&
            strncmp(json + tok->start, s, tok->end - tok->start) == 0);
}

void parse_weather(char *json) {
    jsmn_parser p;
    jsmntok_t t[128];
    jsmn_init(&p);
    int r = jsmn_parse(&p, json, strlen(json), t, 128);

    for (int i = 1; i < r; i++) {
        if (jsmn_eq(json, &t[i], "description")) {
            int len = t[i + 1].end - t[i + 1].start;
            strncpy(weather_desc, json + t[i + 1].start, len);
            weather_desc[len] = '\0';
            break;
        }
    }
}

int main() {
    lcd_init();
    time_t last_weather = 0;

    while (1) {
        time_t now = time(NULL);
        struct tm *tm_info = localtime(&now);
        char line1[17], line2[17];

        snprintf(line1, sizeof(line1), "%02d:%02d %02d-%02d",
                 tm_info->tm_hour, tm_info->tm_min,
                 tm_info->tm_mday, tm_info->tm_mon + 1);

        if (now - last_weather > WEATHER_INTERVAL) {
            char *resp = http_get(LAT, LON, API_KEY);
            if (resp) {
                parse_weather(resp);
                free(resp);
                last_weather = now;
            }
        }

        snprintf(line2, sizeof(line2), "%-16s", weather_desc);
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print(line1);
        lcd_set_cursor(0, 1);
        lcd_print(line2);

        sleep(1);
    }

    return 0;
}

