#pragma once
#include <stdint.h>
#include <Arduino.h>

void led_init(uint32_t pin);

void led_on(uint32_t pin);

void led_off(uint32_t pin);

void led_flip(uint32_t pin);