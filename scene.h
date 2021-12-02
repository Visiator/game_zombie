#pragma once

#include <stdio.h>
#include <vector>

#include "primitives.h"
#include "font.h"

class SCENE;

void fill_poly(uint32_t *screen, int screen_width, Point2Di left_back, Point2Di left, Point2Di right_back, Point2Di right, uint32_t color);

/*
class LINE {
  public:

  Point2Di A, B;

  void paint(uint32_t *screen, int screen_width);

  LINE(int x1, int y1, int x2, int y2) : A(x1,y1) , B(x2, y2) {};
};
*/

class POLYGON {

  public:

  std::vector<Point2Di> points;
  //std::vector<LINE> lines;

  int left_side[800];
  int right_side[800];

  bool in_polygon(int x, int y);

  void paint(uint32_t *screen, int screen_width, uint32_t color, nFONT *fnt);

  void recalc();

  void get_left_right(Point2Di *usr, Point2Di &left, Point2Di &right);

  POLYGON() { for(int i=0;i<800;i++) { left_side[i] = -1; right_side[i] = -1; }  }
};

class USER {
public:
  int score = 0;

  float speed_x = 1, speed_y = -1, speed_max = 5, speed_delta = 0.5;
  void speed_x_plus();
  void speed_x_minus();
  void speed_y_plus();
  void speed_y_minus();
  void calc_move(SCENE *parent);
  int is_immunity = 0;

  int x = 400, y = 400;

  void paint(uint32_t *screen, int screen_width, uint32_t color, uint32_t color_immunity, nFONT *fnt);
  bool is_contacted(int xx, int yy);

  USER(int x, int y) : x(x), y(y) {}
};

class BONUS {
public:
  int x = 400, y = 400;

  void paint(uint32_t *screen, int screen_width, uint32_t color, nFONT *fnt);

  void change_position_random(SCENE *parent);
  bool is_contacted(int xx, int yy);


  BONUS() {}
};

class ZOMBIE {
public:
  float x = 400, y = 400;
  float speed_x = 0, speed_y = 0, speed = 1;
  bool is_visible = false;

  Point2Di target_pont{-1, -1};

  void paint(uint32_t *screen, int screen_width, uint32_t color, uint32_t SCENE_COLOR, nFONT *fnt);

  void change_position_random(SCENE *parent);
  bool is_contacted(int xx, int yy);
  void calc_move(uint32_t *screen, int screen_width, SCENE *parent);

  ZOMBIE(){}
};

class SCENE {

public:
  uint32_t SCENE_COLOR   = 0xff005500;
  uint32_t SHADOW_COLOR  = 0xff003300;
  uint32_t POLYGON_COLOR = 0xff770000;
  uint32_t ZOMBIE_COLOR  = 0xff000099;
  uint32_t USER_COLOR    = 0xff00ffff;
  uint32_t USER_IMMUNITY = 0xff999999;
  uint32_t BONUS_COLOR   = 0xff55ff55;

  BONUS  bonus;
  ZOMBIE zombie;
  USER   user{400, 400};

  std::vector<POLYGON> polygons;

  bool in_polygon(int x, int y);

  void first_init();

  void paint(uint32_t *screen, int screen_width, nFONT *fnt);

  SCENE(){}
};