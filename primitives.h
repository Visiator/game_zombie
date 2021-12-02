#pragma once

#include <vector>
#include <stdio.h>

struct Point2Di
{
    int x = 0;
    int y = 0;
    Point2Di(const int &x, const int &y) : x(x), y(y) {}
    Point2Di(){}
};

void  drawRectangle(uint32_t* screen, const int screen_width, const int x1, const int y1, const int x2, const int y2, uint32_t color);
void  fill_left_right_side(uint32_t *screen, int screen_width, int *left_side, int *right_side, uint32_t color);
void  fill_Polygon(uint32_t* screen, const int screen_width, uint32_t color, int point_count, Point2Di *points[]);
void  drawCircle(uint32_t* screen, int screen_width, int x, int y, int r, uint32_t color);
void  drawLine(uint32_t* screen, const int screen_width, const int x1, const int y1, const int x2, const int y2, const uint32_t color);
void  drawLine_virtual(const int screen_width, const Point2Di A, const Point2Di B, const uint32_t color, int *array_XX );
float angle(Point2Di *A, Point2Di *B);
