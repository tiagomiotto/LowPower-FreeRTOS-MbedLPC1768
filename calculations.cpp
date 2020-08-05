#include "stdio.h"

void fibonnacciCalculation(int cycles)
{
    int a=0,b=1,i;

    for(i=0; i<=cycles; i++) {
        int nextTerm= a+b;
        a=b;
        b=nextTerm;
    }
}

void primeCalculation(int cycles)
{
    int count, c,i=3;
    for (count = 1; count <= cycles;) {
        for (c = 2; c <= i - 1; c++) {
            if (i%c == 0)
                break;
        }
        if (c == i) {
            count++;
        }
        i++;
    }
}
void IOCalculation(int aux)
{


}