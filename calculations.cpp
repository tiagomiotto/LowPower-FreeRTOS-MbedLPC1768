#include "stdio.h"
#include "calculations.h"

long fibonnacciCalculation(long cycles)
{
    long a=1, b=1, i;

    for (i=0; i<=cycles; i++) {
        long nextTerm= a+b;
        a=b;
        b=nextTerm;
    }
    return a;
}

// void primeCalculation(int cycles)
// {
//     int count, c, i=3;
//     for (count = 1; count <= cycles;) {
//         for (c = 2; c <= i - 1; c++) {
//             if (i%c == 0)
//                 break;
//         }
//         if (c == i) {
//             count++;
//         }
//         i++;
//     }
// }
// void IOCalculation(int aux)
// {


// }

// long longLongCalculation(int cycles) {
//     float a = 123;
//     float b = 23;
//     float c = 341500;
//     long i =1, j,k;
//     for (i = 1; i <= 50000; i++) {
        
//         // a= a/b;
//         // a= a*b;
//         a++;
//         a--;
//         //for (j = 1; j <= 100; j++) longLongCalculation(1);
//         // for (j = 1; j <= 10000000000; j++) {
//         //     a = a - (b % 2);
//         //     b = b - (c % 2);
//         //     a= a/b;
//         //     a= a*b;
//         //             for (k = 1; k <= 10000000000; k++) {
//         //     a = a - (b % 2);
//         //     b = b - (c % 2);
//         //     a= a/b;
//         //     a= a*b;
//         // }
//         }
        
    
//     return a;
// } 
// long longCalculation(int cycles) {
//     long a = 100234555;
//     long b = 22333335;
//     long c = 341500;
//     long i =1, j,k;
//     for (i = 1; i <= 10000000000; i++) {
//         a = a - (b % 2);
//         b = b - (c % 2);
          
//     }
//     //while(1);
//     return i;
// }