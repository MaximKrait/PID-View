#include <stdio.h>
#include <stdbool.h>

int full_massive_char(char arr[], int memory) {
    
    for (int i = 0; i < memory; i++) {
        if (arr[i] == '\0') {
            return 0; 
        }
    }
    return 1;
}