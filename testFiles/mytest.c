#include <stdio.h>
int mytest (int a) {
    int num;
    int b = 4;
    int c, d, e;

    num = b * 2;
    e = 4 + num;
    num = e-5;
    d = 6+num;
    if (e > 10) {
        d = 5;
    } else {
        d = 10;
    }

    return d;
}
