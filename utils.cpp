//
// Created by mtriston on 02.05.2021.
//

#include "utils.hpp"

std::string cut_next_token(std::string &str, std::string const &delim) {
    std::string token = str.substr(0, str.find(delim));
    if (!token.empty()) {
        str.erase(0, token.size() + delim.length());
    }
    return token;
}

int ft_atoi(std::string const &str) {
    int res = 0;
    int sign = 1;
    int i = 0;
    if (str[0] == '-') {
        sign = -1;
        i++;
    }
    for (; str[i] != '\0'; ++i)
        res = res * 10 + str[i] - '0';

    return sign * res;
}

std::string ft_tolower(std::string const &str) {
    std::string result(str);
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

static void write_number(char *dest, unsigned int n, int i) {
    if (n >= 10) {
        write_number(dest, n / 10, i - 1);
        dest[i] = n % 10 + '0';
    } else
        dest[i] = n + '0';
}

char *ft_itoa(int n) {
    int i;
    int num;
    unsigned int temp_num;
    char *str;

    num = n;
    i = (n < 0 || n == 0) ? 1 : 0;
    while (num != 0) {
        num /= 10;
        i++;
    }
    if (!(str = (char *) malloc((i + 1) * sizeof(char))))
        return (0);
    if (n < 0) {
        str[0] = '-';
        temp_num = -n;
    } else
        temp_num = n;
    write_number(str, temp_num, i - 1);
    str[i] = '\0';
    return (str);
}

std::string convertTime(const time_t *t) {
    struct tm *timeinfo;
    char buffer[80] = {};

    timeinfo = gmtime(t);
    strftime(buffer, 80, "%a, %d %b %Y %T %Z", timeinfo);
    return std::string(buffer);
}