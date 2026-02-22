#include "Stack.h"
#include <iostream>
#include <stdlib.h> 
Stack::Stack() : sp { -1 }{
}

void Stack::push(uint16_t value) {
    if (sp < STACK_SIZE - 1) {
        data[++sp] = value;
    } else {
        std::cerr << "Error: Stack Overflow! The program will now exit.\n";
        exit(EXIT_FAILURE); // Terminates the program with an error code
    }
}

uint16_t Stack::pop() {
    if (sp >= 0) {
        return data[sp--];
    } else {
        std::cerr << "Error: Stack Underflow! The program will now exit.\n";
        exit(EXIT_FAILURE);
        return 0;
    }
}