#include "misc.hpp"

double lintrp(double x, double x0, double x1, double y0, double y1){
    return y0 + ((y1 - y0)/(x1 - x0)) * (x - x0);
};