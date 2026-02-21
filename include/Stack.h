#ifndef STACK_H
#define STACK_H

#include <stdint.h>

class Stack {
public:
    Stack();
    void push(uint16_t value);
    uint16_t pop();

private:
    static const int STACK_SIZE = 16;
    uint16_t data[STACK_SIZE];
    int8_t sp; // Stack pointer
};

#endif