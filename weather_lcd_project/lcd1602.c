#include "lcd1602.h"
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define BCM2835_PERI_BASE       0x3F000000
#define GPIO_BASE               (BCM2835_PERI_BASE + 0x200000)

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

volatile unsigned int *gpio;

#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define GPIO_SET *(gpio+7)
#define GPIO_CLR *(gpio+10)

#define GPIO_SET_PIN(g) GPIO_SET = 1 << g
#define GPIO_CLR_PIN(g) GPIO_CLR = 1 << g

#define GPIO_RS 17
#define GPIO_E  18
#define GPIO_D4 27
#define GPIO_D5 22
#define GPIO_D6 23
#define GPIO_D7 24

void gpio_setup() {
    int mem_fd = open("/dev/mem", O_RDWR|O_SYNC);
    if (mem_fd < 0) {
        perror("open");
        return;
    }

    void *gpio_map = mmap(
        NULL,
        BLOCK_SIZE,
        PROT_READ|PROT_WRITE,
        MAP_SHARED,
        mem_fd,
        GPIO_BASE
    );

    close(mem_fd);

    if (gpio_map == MAP_FAILED) {
        perror("mmap");
        return;
    }

    gpio = (volatile unsigned int *)gpio_map;

    int pins[] = { GPIO_RS, GPIO_E, GPIO_D4, GPIO_D5, GPIO_D6, GPIO_D7 };
    for (int i = 0; i < 6; i++) {
        INP_GPIO(pins[i]);
        OUT_GPIO(pins[i]);
    }
}

void pulse_enable() {
    GPIO_SET_PIN(GPIO_E);
    usleep(1);
    GPIO_CLR_PIN(GPIO_E);
    usleep(50);
}

void write_nibble(uint8_t nibble) {
    (nibble & 0x1) ? GPIO_SET_PIN(GPIO_D4) : GPIO_CLR_PIN(GPIO_D4);
    (nibble & 0x2) ? GPIO_SET_PIN(GPIO_D5) : GPIO_CLR_PIN(GPIO_D5);
    (nibble & 0x4) ? GPIO_SET_PIN(GPIO_D6) : GPIO_CLR_PIN(GPIO_D6);
    (nibble & 0x8) ? GPIO_SET_PIN(GPIO_D7) : GPIO_CLR_PIN(GPIO_D7);
    pulse_enable();
}

void lcd_write_byte(uint8_t byte, int is_data) {
    if (is_data)
        GPIO_SET_PIN(GPIO_RS);
    else
        GPIO_CLR_PIN(GPIO_RS);

    write_nibble(byte >> 4);
    write_nibble(byte & 0x0F);
    usleep(50);
}

void lcd_send_cmd(uint8_t cmd) {
    lcd_write_byte(cmd, 0);
}

void lcd_send_data(uint8_t data) {
    lcd_write_byte(data, 1);
}

void lcd_init() {
    gpio_setup();
    usleep(50000); // wait for LCD to power up

    GPIO_CLR_PIN(GPIO_RS);
    GPIO_CLR_PIN(GPIO_E);

    write_nibble(0x03); usleep(4500);
    write_nibble(0x03); usleep(4500);
    write_nibble(0x03); usleep(150);
    write_nibble(0x02); // 4-bit mode

    lcd_send_cmd(0x28); // 4-bit, 2 line, 5x8 dots
    lcd_send_cmd(0x08); // display off
    lcd_send_cmd(0x01); // clear display
    usleep(2000);
    lcd_send_cmd(0x06); // entry mode
    lcd_send_cmd(0x0C); // display on, cursor off
}

void lcd_clear() {
    lcd_send_cmd(0x01);
    usleep(2000);
}

void lcd_set_cursor(int col, int row) {
    static uint8_t row_offsets[] = { 0x00, 0x40 };
    lcd_send_cmd(0x80 | (col + row_offsets[row]));
}

void lcd_print(const char *str) {
    while (*str) {
        lcd_send_data(*str++);
    }
}
