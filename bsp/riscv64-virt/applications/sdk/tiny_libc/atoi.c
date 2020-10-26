#include <stdlib.h>

int atoi(char* pstr) {
    /* do not use it for signed numbers! */
    int ret = 0;              
    if (pstr == 0) return 0;
    while(' '== *pstr) pstr++;
    while(1) {
        if (*pstr >= '0' && *pstr <= '9') {
            ret = ret * 10 + (((int)*pstr) - '0');
            pstr++;
        } else if (*pstr != 0) {
            return -1;
        } else {
            break;
        }
        
    }
    return ret;
}
