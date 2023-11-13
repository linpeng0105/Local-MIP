#include <cstring>
#include <fstream>
#include "header.h"
#include "paras.h"

double paras::identify_opt(const char *file)
{
    char name[strlen(file) + 1], p = -1, l = strlen(file);
    for (int i = l - 1; i >= 0; i--)
        if (file[i] == '/')
        {
            p = i;
            break;
        }
    strncpy(name, file + p + 1, l - p - 1);
    name[l - p - 1] = '\0';
    printf("c File name (with path): %s\n", file);
    printf("c File name: %s\n", name);
    return minValue;
}

bool LessEqual(double a, double b)
{
    return Equal(a, b) || L(a, b);
}

bool GreaterEqual(double a, double b)
{
    return Equal(a, b) || Greater(a, b);
}

bool Equal(double a, double b)
{
    return abs(a - b) <= Epsilon;
}

bool L(double a, double b)
{
    return a < b - Epsilon;
}

bool Greater(double a, double b)
{
    return a - Epsilon > b;
}