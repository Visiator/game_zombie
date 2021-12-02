#include <stdio.h>
#include <cmath>

#include "primitives.h"
#include "scene.h"

using namespace std;


template <class T> void sswap ( T& a, T& b ) { T c(a); a=b; b=c; }   

void drawRectangle(uint32_t* screen, const int screen_width, int x, int y, int w, int h, uint32_t color) {
    
    if(x < 1) x = 1;
    if(y < 1) y = 1;
    uint32_t *q;
    for(int i=0; i<h; i++) {
      q = screen + (y+i)*screen_width + x;
      for(int j=0; j<w; j++) *q++ = color;
    }
}

void drawLine(uint32_t* screen, const int screen_width, const int x1, const int y1, const int x2, const int y2, const uint32_t color) {
    // рисуем линию. взято из интернета. красивое.

    int dx = x2 - x1;
    int dy = y2 - y1;

    int dLong = abs(dx);
    int dShort = abs(dy);

    int offsetLong = dx > 0 ? 1 : -1;
    int offsetShort = dy > 0 ? screen_width : -screen_width;

    if(dLong < dShort)
    {
        sswap(dShort, dLong);
        sswap(offsetShort, offsetLong);
    }

    int error = dLong/2;
    int index = y1*screen_width + x1;
    const int offset[] = {offsetLong, offsetLong + offsetShort};
    const int abs_d[]  = {dShort, dShort - dLong};
    for(int i = 0; i <= dLong; ++i)
    {
        screen[index] = color;  // or a call to your painting method
        const int errorIsTooBig = error >= dLong;
        index += offset[errorIsTooBig];
        error += abs_d[errorIsTooBig];
    }
}


void drawLine_virtual(const int screen_width, const Point2Di A, const Point2Di B, const uint32_t color, int *array_XX ) {
  // нужно заполнить массив array_XX координатами точек

  int dx = B.x - A.x;
  int dy = B.y - A.y;

  int ddx = abs(dx);
  int ddy = abs(dy);

  int offset_XX = dx > 0 ? 1 : -1;
  int offset_YY = dy > 0 ? screen_width : -screen_width;
  int offset_YYY = dy > 0 ? 1 : -1;

  int xx, yy;
  int e, error_ddx, error_ddy;

  if(ddx > ddy)
  {

    error_ddx = ddx/2;
    //int index = A.y*imageSide + A.x;
    const int offset[] = {offset_XX, offset_XX + offset_YY};
    const int abs_d[]  = {ddy, ddy - ddx};

    xx = A.x;
    yy = A.y;

    for(int i = 0; i <= ddx; ++i)
    {
        //screen[index] = color;  // or a call to your painting method
        const int errorIsTooBig = error_ddx >= ddx;

        if( errorIsTooBig == 0 ) {
          xx += offset_XX;
        } else {
          xx += offset_XX;
          yy += offset_YYY;
        }

        array_XX[yy] = xx;
        //index += offset[errorIsTooBig];
        e = abs_d[errorIsTooBig];
        error_ddx += e;        
    }
  } else {

    error_ddy = ddy/2;
    //int index = A.y*imageSide + A.x;
    const int offset[] = {offset_YY, offset_XX + offset_YY};
    const int abs_d[]  = {ddx, ddx - ddy};

    xx = A.x;
    yy = A.y;
   
    for(int i = 0; i <= ddy; ++i)
    {
        //screen[index] = color;  // or a call to your painting method
        const int errorIsTooBig = error_ddy >= ddy;
        if( errorIsTooBig == 0 ) {
          yy += offset_YYY;
        } else {
          yy += offset_YYY;
          xx += offset_XX;

        }

        array_XX[yy] = xx;
        //index += offset[errorIsTooBig];
        e = abs_d[errorIsTooBig];
        error_ddy += e;
    }
  }
}


void drawCircle(uint32_t* screen, int screen_width, int x, int y, int r, uint32_t color) {
  if (r < 1) return;

  int y1 = y - r;
  if (y1 < 0) y1 = 0;
  int y2 = y + r;
  if (y2 >= screen_width) y2 = screen_width-1;
 
  int x1 = x - r;
  if (x1 < 0) x1 = 0;
  int x2 = x + r;
  if (x2 >= screen_width) x2 = screen_width-1;

  if ((x2 < x1) || (y2 < y1)) return;

  int rr = r * r + r;
  int dxdx0 = (x1 - x)*(x1 - x);
  int ab0 = 2 * (x1 - x) + 1;

  for( int i = y1; i <= y2; i++) {

    int dd = (i - y)*(i - y) + dxdx0;
    int ab = ab0;
    uint32_t *cc = &screen[i*screen_width + x1];
    for(int d = x1 - x2; d <= 0; d++ ) {
      if (dd < rr) *cc = color;
      cc++;
      dd += ab;
      ab += 2;
    }
  }
}

void fill_left_right_side(uint32_t *screen, int screen_width, int *left_side, int *right_side, uint32_t color) {
  // заливаем полигон горизонтальными линиями, координаты берем из массивов left_side, right_side

  uint32_t *q;
  int x, xx;

  for(int i=0; i<screen_width; i++) {
       
    if(left_side[i] != -1 && right_side[i] != -1) {
      x  = left_side[i];
      xx = right_side[i];
      q = screen + i*screen_width + x;
      while(x < xx) { 
        *q++ = color;
        x++;
      }
    }
  }
}

void fill_Polygon(uint32_t* screen, const int screen_width, uint32_t color, int point_count, Point2Di *points[]) {
  // к нам приходит ВЫПУКЛЫЙ полигон в виде массива вершин (точек)
  // мы точно знаем, что вершины внутри массива упорядочены по принципу "против часовой стрелки"
  // а это значит, что если ребро расположено сверху->вниз, то это левый бок полигона, а если ребро расположено снизу->вверх, то это правый бок полигона
  // закрашивать полигон будем горизантальными линиями от левого бока полигона к правому боку,
  // для этого предварительно заполним два массива с координатами линий (начало и конец горизонтальной линии)

  int i, a = 0;

  int left_side[screen_width], right_side[screen_width];

  for(int i=0; i<screen_width; i++) { left_side[i] = -1; right_side[i] = -1; };

  for(i=1; i<point_count; i++) {
    if(points[i]->y > points[i-1]->y) drawLine_virtual(screen_width, *points[i-1], *points[i], 0, left_side); // эта функция не рисует линию, только сохраняет координаты точек в массив
    else       drawLine_virtual(screen_width, *points[i-1], *points[i], 0, right_side);   
  }
  if(points[0]->y > points[i-1]->y)  drawLine_virtual(screen_width, *points[i-1], *points[0], 0, left_side);
  else        drawLine_virtual(screen_width, *points[i-1], *points[0], 0, right_side);    

  fill_left_right_side(screen, screen_width, left_side, right_side, color); // заливаем горизонтальные линии, координаты берем из массивов left_side, right_side
}

float angle(Point2Di *A, Point2Di *B) {
  // определяем угол наклона. условно, в категории больше - меньше

  float dx, dy, d;

  if(B->x >= A->x && B->y <= A->y) { // 0 - 90
    dx = B->x - A->x;
    dy = A->y - B->y;
    if(B->y == A->y) {
      d = 0;
    } else {
      if(A->x == B->x) {
        d = 0 + 90;
      } else {
        d = 0 + (dy / dx) / 112.0; // тут все правильно. (если макс разрешение будет 10000 pix, то нужно 10000 привести к 90, 10000/90 ~= 112
      }
    }
    return d;
  }

  if(B->x <= A->x && B->y <= A->y) { // 90 - 180
    dx = A->x - B->x;
    dy = A->y - B->y;
    if(B->y == A->y) {
      d = 180;
    } else {
      if(A->x == B->x) {
        d = 90;
      } else {
        d = 90 + (dx / dy) / 112.0;
      }
    }
    return d;
  }

  if(B->x <= A->x && B->y >= A->y) { // 180 - 270
    dx = A->x - B->x;
    dy = B->y - A->y;
    if(B->y == A->y) {
      d = 180;
    } else {
      if(A->x == B->x) {
        d = 180 + 90;
      } else {
        d = 180 + (dy / dx) / 112.0; // тут все правильно. (если макс разрешение будет 10000 pix, то нужно 10000 привести к 90, 10000/90 ~= 112
      }
    }
    return d;
  }
  if(B->x >= A->x && B->y >= A->y) {
    dx = B->x - A->x;
    dy = B->y - A->y;
    if(B->x == A->x) {
      d = 270;
    } else {
      if(B->y == A->y) {
        d = 270 + 90;
      } else {
        d = 270 + (dx / dy) / 112.0;
      }
    }
    return d;
  }

  return 0;
}
