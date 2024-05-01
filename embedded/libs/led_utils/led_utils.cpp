#include "led_utils.h"

void led_init(uint32_t pin){
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void led_on(uint32_t pin){
    digitalWrite(pin, HIGH);
}

void led_off(uint32_t pin){
    digitalWrite(pin, LOW);
}

void led_flip(uint32_t pin){
    if(digitalRead(pin) == HIGH){
        digitalWrite(pin, LOW);
    }
    else{
        digitalWrite(pin, HIGH);
    }
}