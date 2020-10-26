long int atol(const char* str)
{
    if (str[0] == '\0') return 0;
    if (str[1] == '\0') return str[0] - '0';

    int base = 10;
    if ((str[0] == '0' && str[1] == 'x') ||
        (str[0] == '0' && str[1] == 'X')) {
        base = 16;
        str += 2;
    }
    long ret = 0;
    while (*str != '\0') {
        if ('0' <= *str && *str <= '9') {
            ret = ret * base + (*str - '0');
        } else if (base == 16) {
            if ('a' <= *str && *str <= 'f'){
                ret = ret * base + (*str - 'a') + 10;
            } else if ('A' <= *str && *str <= 'F') {
                ret = ret * base + (*str - 'A') + 10;
            } else {
                return 0;
            }
        } else {
            return 0;
        }
        ++str;
    }
    return ret;
}
