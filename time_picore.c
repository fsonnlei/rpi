#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <wiringPi.h>
#include <lcd.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include <sys/stat.h>

#define API_KEY     "f96a8d9254b8190100653a337a99badc"
#define LAT 		"47.6854"
#define LON 		"-122.1621"
#define UNITS       "imperial"
#define API_URL_FMT "http://api.openweathermap.org/data/2.5/weather?lat=%s&lon=%s&appid=%s&units=%s"

// http://api.openweathermap.org/data/2.5/weather?lat={LAT}&lon={LON}&appid={WEATHER_API_KEY}&units=imperial&lang=en

#define LCD_RS  3
#define LCD_E   0
#define LCD_D4  6
#define LCD_D5  1
#define LCD_D6  5
#define LCD_D7  4

#define UPDATE_INTERVAL 1800 // 30 minutes

int lcd;

struct string {
    char *ptr;
    size_t len;
};

void init_string(struct string *s) {
    s->len = 0;
    s->ptr = malloc(1);
    if (s->ptr == NULL) exit(EXIT_FAILURE);
    s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) {
    size_t new_len = s->len + size * nmemb;
    s->ptr = realloc(s->ptr, new_len + 1);
    if (s->ptr == NULL) exit(EXIT_FAILURE);
    memcpy(s->ptr + s->len, ptr, size * nmemb);
    s->len = new_len;
    s->ptr[new_len] = '\0';
    return size * nmemb;
}

char* fetch_weather(const char *lat, const char *lon, const char *apikey) {
    CURL *curl;
    CURLcode res;
    struct string s;
    init_string(&s);

    char url[256];
	snprintf(url, sizeof(url), API_URL_FMT, lat, lon, apikey, UNITS);

    curl = curl_easy_init();
    if (!curl) return NULL;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        free(s.ptr);
        return NULL;
    }

    return s.ptr;
}

char* parse_weather_description(const char *json_str) {
    struct json_object *parsed_json, *weather_arr, *weather, *description;
    parsed_json = json_tokener_parse(json_str);
    if (!parsed_json) return strdup("Weather err");

    json_object_object_get_ex(parsed_json, "weather", &weather_arr);
    weather = json_object_array_get_idx(weather_arr, 0);
    json_object_object_get_ex(weather, "description", &description);

    return strdup(json_object_get_string(description));
}

void display_on_lcd(const char *line1, const char *line2) {
    lcdClear(lcd);
    lcdPosition(lcd, 0, 0);
    lcdPuts(lcd, line1);
    lcdPosition(lcd, 0, 1);
    lcdPuts(lcd, line2);
}

void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS); // parent exits

    setsid(); // become session leader
    umask(0);
    chdir("/");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main() {
    daemonize();

    wiringPiSetup();
    lcd = lcdInit(2, 16, 4, LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0);

    char *weather_desc = strdup("Fetching...");
    time_t last_update = 0;

    while (1) {
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char time_str[17];
        strftime(time_str, sizeof(time_str), "%H:%M %d-%m", t);

        if (difftime(now, last_update) >= UPDATE_INTERVAL) {
			char *json_str = fetch_weather(LAT, LON, API_KEY);
            if (json_str) {
                free(weather_desc);
                weather_desc = parse_weather_description(json_str);
                free(json_str);
            }
            last_update = now;
        }

        display_on_lcd(time_str, weather_desc);
        sleep(1);
    }

    free(weather_desc);
    return 0;
}
