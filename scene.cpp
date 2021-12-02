
#include <vector>
#include <math.h>
#include "scene.h"
#include <iostream>
#include <fstream>

#include "font.h"
#include "primitives.h"

using namespace std;

vector<string> my_split(const string& str, int delimiter(int) = ::isspace){
  vector<string> result;
  auto e = str.end();
  auto i = str.begin();
  while(i != e){
    i=find_if_not(i, e, delimiter);
    if(i == e) break;
    auto j = find_if(i, e, delimiter);
    result.push_back(string(i, j));
    i = j;
  }
  return result;
}


void SCENE::first_init() {
    // загрузим полигоны из файла

    ifstream fin;
    fin.open("conf/poligons.conf");
    string FileLine;
 
    if (fin.is_open()){

        int line_no = 0;
        while(getline(fin, FileLine)){ // Считываем поочерёдно строки из файла
            
            polygons.push_back( POLYGON() );

            vector<string> FileWord = my_split(FileLine);

            int v1 = -1, v2 = -1, val = -1;
            for(auto w : FileWord) {
                
                val = atoi(w.c_str());
                if( v1 == -1 ) v1 = val;
                else v2 = val;
                if( v1 != -1 && v2 != -1 ) {
                  polygons[line_no].points.push_back( Point2Di(v1, v2) );
                  v1 = -1;
                  v2 = -1;
                }
                
            }
            polygons[line_no].recalc();
            line_no++; 
        }
    }
    else
        cout << "open file error" << endl;

  zombie.change_position_random(this);
  bonus.change_position_random(this);

}



void POLYGON::paint(uint32_t *screen, int screen_width, uint32_t color, nFONT *fnt) {
   fill_left_right_side(screen, screen_width, left_side, right_side, color);
}




Point2Di* _for_sort_A0 = nullptr;

bool points_comp (Point2Di a, Point2Di b) {
  return angle(_for_sort_A0, &a) < angle(_for_sort_A0, &b);
}


void POLYGON::recalc() {
  if(points.size() < 3) return;

  // упорядочим вершины

  int min_y = 99999, min_idx = -1, i, j;

  // найдем самую верхнюю точку
  i = 0;
  for(auto p : points) {
    if(p.y < min_y) {
      min_y = p.y;
      min_idx = i;
    }
    i++;
  }
  if(min_idx > 0) {
    swap(points[0], points[min_idx]);
  }

  // отсортируем точки по признаку "угол наклона к верхней(нулевой) точке"
  _for_sort_A0 = &points[0];
  sort( points.begin()+1, points.end(), &points_comp);

  // для того, чтобы в последующем мы могли быстро заливать полигон, приготовим два массива с коодинатами левого бока полигона и правого
  int a = 0;
  for(i = 1; i<points.size(); i++) {
    if(points[i].y > points[i-1].y) drawLine_virtual(800, points[i-1], points[i], 0xff0000ff, left_side);
    else       drawLine_virtual(800, points[i-1], points[i], 0xffff0000, right_side);
    
  }
  if(points[0].y > points[i-1].y) drawLine_virtual(800, points[i-1], points[0], 0xff0000ff, left_side);
  else       drawLine_virtual(800, points[i-1], points[0], 0xffff0000, right_side);
 

}


void POLYGON::get_left_right(Point2Di *usr, Point2Di &r, Point2Di &l) {
  // найдем две точки полигона которые максимально разнесенны относительно направления взгляда игрока


  // проверим, пересекает ли нулевая ось наш полигон
  // нулевая ось это луч из точки usr строго вправо
  int f360 = 0; 
  if( right_side[usr->y] > usr->x ) f360 = 1;

  r.x = points[0].x;
  r.y = points[0].y;
  l.x = points[0].x;
  l.y = points[0].y;

  float an, an_min = 999999, an_max = -1;

  for(int i=0; i < points.size(); i++) {

    an = angle(usr, &points[i]); // угол взгляда игрока

    if(f360 == 1 && an <= 180) an += 360.0;
    if( an < an_min ) {
      an_min = an;
      r.x = points[i].x;
      r.y = points[i].y;
    }
    if( an > an_max ) {
      an_max = an;
      l.x = points[i].x;
      l.y = points[i].y;
    }
  }
}

#define TOP_LEFT     1
#define LEFT_TOP     2
#define LEFT_BOTTOM  3
#define BOTTOM_LEFT  4
#define BOTTOM_RIGHT 5
#define RIGHT_BOTTOM 6
#define RIGHT_TOP    7
#define TOP_RIGHT    8

//-------------------------------------------------------------------------//
void calc_back_point(Point2Di user, Point2Di &point, Point2Di &point_back, int &mode) {
  // найдем для точки ее проекцию на границу игрового поля относительно вгляда игрока

  point_back.x = 0;
  point_back.y = 0;
  mode = 0;

     float k, xx, dx, dy, dx1, dy1;
     
     if(user.x == point.x) {
       if(user.y < point.y) {
         point_back.x = user.x;
         point_back.y = 800 - 1;
         mode = BOTTOM_RIGHT;
       }
       if(user.y > point.y) {
         point_back.x = user.x;
         point_back.y = 1;
         mode = TOP_LEFT;

       }
     }
     if(user.y == point.y) {
       if(user.x < point.x) {
         point_back.x = 800 - 1;
         point_back.y = user.y;
         mode = RIGHT_TOP;
       }
       if(user.x > point.x) {
         point_back.x = 1;
         point_back.y = user.y;
         mode = LEFT_BOTTOM;
       }

     }
     

     if(user.y > point.y && user.x > point.x) {

       dx = user.x - point.x;
       dy = user.y - point.y;

       k = dx / dy;

       dy1 = user.y;
       dx1 = user.x - k * dy1;

       if( dx1 >= 0 && dx1 < 800) {
         point_back.x = dx1; if(point_back.x == 0) point_back.x = 1;
         point_back.y = 1;
         mode = TOP_LEFT;
       } else {
         k = dy / dx;
         dx1 = user.x;
         dy1 = user.y - k * dx1;
         point_back.x = 1;
         point_back.y = dy1;
         mode = LEFT_TOP;
       }
     }
     if(user.y < point.y && user.x > point.x) {

       dx = user.x - point.x;
       dy = point.y - user.y;

       k = dy / dx;
       dx1 = user.x;
       dy1 = user.y + k * dx1; if(dy1==0) dy1 = 1;
       
       if( dy1 >= 0 && dy1 < 800 ) { 
         point_back.x = 1;
         point_back.y = dy1; if(point_back.y==0) point_back.y= 1;
         mode = 3;
       } else {
         k = dx / dy;
         dy1 = (800 - user.y);
         dx1 = user.x - k * dy1; 
         if( dx1 >= 0 && dx1 < 800 ) {
           point_back.x = dx1; if(point_back.x==0) point_back.x=1;
           point_back.y = 800-1;
           mode = BOTTOM_LEFT;
         }       
       }
     }
     if(user.y < point.y && user.x < point.x) {
         dx = point.x - user.x;
         dy = point.y - user.y;

         k = dx / dy;
         dy1 = (800 - user.y);
         dx1 = user.x + k * dy1;
         if( dx1 >= 1 && dx1 < 800 ) {
           point_back.x = dx1;
           point_back.y = 800-1;
           mode = BOTTOM_RIGHT;
         } else {
           k = dy / dx;
           dx1 = (800 - user.x);
           dy1 = user.y + k * dx1;
           point_back.x = 800-1;
           point_back.y = dy1;
           mode = RIGHT_BOTTOM;
         }
     }
     if(user.y > point.y && user.x < point.x) {
         dx = point.x - user.x;
         dy = point.y - user.y;

         k = dy / dx;
         dx1 = (800 - user.x);
         dy1 = user.y + k * dx1;

         if( dy1 >= 1 && dy < 800 ) {
           point_back.x = 800-1;
           point_back.y = dy1;
           mode = RIGHT_TOP;
         } else {
           k = dx / dy;
           dy1 = user.y;
           dx1 = user.x - k * dy1;
           if( dx1 >= 1 && dx < 800 ) {
             point_back.x = dx1;
             point_back.y = 1;
             mode = TOP_RIGHT;
           }
         }
      }     
}

bool POLYGON::in_polygon(int x, int y) {
  // принадлежит ли точка полигону ?
  if( x <= 5 || x >= 800 -5 || y <= 5 || y >= 800 - 5 ) return true;
  if(left_side[y] != -1 && left_side[y] <= x && right_side[y] != -1 && right_side[y] >= x ) return true;
  return false;
}

//==============================================================================================================//
//  USER

void USER::speed_x_plus()  { if(speed_x<speed_max) speed_x += speed_delta; }
void USER::speed_x_minus() { if(speed_x>speed_max*-1) speed_x -= speed_delta; }
void USER::speed_y_plus()  { if(speed_y<speed_max) speed_y += speed_delta; }
void USER::speed_y_minus() { if(speed_y>speed_max*-1) speed_y -= speed_delta; }

void USER::calc_move(SCENE *parent) {
  int xx, yy;

  xx = x + speed_x;
  yy = y + speed_y;

  
  if( parent->in_polygon(xx, yy) ) {
    speed_x *= -1;
    speed_y *= -1;
    return;
  }

  if( parent->bonus.is_contacted(xx, yy) ||
      parent->bonus.is_contacted(xx-6, yy) ||
      parent->bonus.is_contacted(xx, yy-6) ||
      parent->bonus.is_contacted(xx+6, yy) ||
      parent->bonus.is_contacted(xx, yy+6) ) {
    score++;
    parent->bonus.change_position_random(parent);

  }

  if(xx <= 10 || xx >= 800-10 || yy <= 10 && yy >= 800-10) {
    speed_x = 0;
    speed_y = 0;

    return;
  }

  if( parent->in_polygon(xx, yy) ) {
    if( speed_x > 0 && speed_y > 0 && parent->in_polygon(xx+1, yy+1) == false ) {
      x = xx+1;
      y = yy+1;
      return;
    };
    if( speed_x > 0 && speed_y < 0 && parent->in_polygon(xx+1, yy-1) == false ) {
      x = xx+1;
      y = yy-1;
      return;
    };
    if( speed_x < 0 && speed_y < 0 && parent->in_polygon(xx-1, yy-1) == false ) {
      x = xx-1;
      y = yy-1;
      return;
    };
    if( speed_x < 0 && speed_y > 0 && parent->in_polygon(xx-1, yy+1) == false ) {
      x = xx-1;
      y = yy+1;
      return;
    };
  };

  x = xx;
  y = yy;

}

bool USER::is_contacted(int xx, int yy) {

  if( x-6 <= xx && x+6 >= xx && y-6 <= yy && y+6 >= yy ) {
    return true;
  }
  return false;
}

void USER::paint(uint32_t *screen, int screen_width, uint32_t color, uint32_t color_immunity, nFONT *fnt) {

  if( is_immunity > 0 ) drawCircle(screen, 800, x, y, 8, color_immunity);
  else                  drawCircle(screen, 800, x, y, 8, color);
}


//==============================================================================================================//
//  ZOMBIE

void ZOMBIE::paint(uint32_t *screen, int screen_width, uint32_t color, uint32_t SCENE_COLOR, nFONT *fnt) {
  if( x < 10 || x > 800 - 10 || y < 10 || y > 800 - 10 ) return;
  if( screen[(int)y * screen_width + (int)x] == SCENE_COLOR) {
    is_visible = true;
    drawRectangle(screen, screen_width, (int)x-6, (int)y-6, 13, 13, color);
  } else {
    is_visible = false;
  }
}

void ZOMBIE::change_position_random(SCENE *parent) {
 
  int xx, yy;
  do
  {
    xx = 0;
    yy = 0;

    while( xx < 30 || xx > 800-30 ) xx = rand() % 800; 
    while( yy < 30 || yy > 800-30 ) yy = rand() % 800; 

    if( parent->in_polygon( xx, yy ) ||
        parent->in_polygon( xx-6, yy ) ||
        parent->in_polygon( xx, yy-6 ) ||
        parent->in_polygon( xx-6, yy-6 ) )
    {
       xx = 0;
       yy = 0;
    }
  }
  while( xx==0 || yy == 0 );
  
  x = xx;
  y = yy;  
}

void ZOMBIE::calc_move(uint32_t *screen, int screen_width, SCENE *parent) {

  if( parent->user.score == 0 ) return;
  if( parent->user.is_immunity > 0 ) return;

  if( is_visible ) {

    target_pont.x = parent->user.x;
    target_pont.y = parent->user.y;
    speed = 1 + parent->user.score*0.45;
  } else {
    speed = 1;
  };
  float k, new_speed_x, new_speed_y;

  
  k = (target_pont.x - x) / (target_pont.y - y);
  new_speed_y = sqrt( speed * speed / ( k*k + 1 ) );
  new_speed_x = sqrt( speed * speed - new_speed_y * new_speed_y );
  if( target_pont.x < x ) new_speed_x *= -1;
  if( target_pont.y < y ) new_speed_y *= -1;

  float inertia = 0.08 * parent->user.score/5;

  if( speed_x < new_speed_x ) { speed_x += inertia; if( speed_x > new_speed_x ) speed_x = new_speed_x; };
  if( speed_x > new_speed_x ) { speed_x -= inertia; if( speed_x < new_speed_x ) speed_x = new_speed_x; };
  if( speed_y < new_speed_y ) { speed_y += inertia; if( speed_y > new_speed_y ) speed_y = new_speed_y; };
  if( speed_y > new_speed_y ) { speed_y -= inertia; if( speed_y < new_speed_y ) speed_y = new_speed_y; };

  float xx, yy;
  xx = x + speed_x;
  yy = y + speed_y;

  if( parent->user.is_immunity == 0 && parent->user.is_contacted(xx, yy) ) {
    parent->user.score /= 2;
    parent->user.is_immunity = 100;
    return;
  }

  if( xx <= 10 || xx >= 800-10 || yy <= 10 || yy >= 800 - 10 ) return;

  if( parent->in_polygon(xx, yy) ) {
    if( speed_x > 0 && speed_y > 0 && parent->in_polygon(xx+1, yy+1) == false ) {
      x = xx+1;
      y = yy+1;
      return;
    };
    if( speed_x > 0 && speed_y < 0 && parent->in_polygon(xx+1, yy-1) == false ) {
      x = xx+1;
      y = yy-1;
      return;
    };
    if( speed_x < 0 && speed_y < 0 && parent->in_polygon(xx-1, yy-1) == false ) {
      x = xx-1;
      y = yy-1;
      return;
    };
    if( speed_x < 0 && speed_y > 0 && parent->in_polygon(xx-1, yy+1) == false ) {
      x = xx-1;
      y = yy+1;
      return;
    };
  };
  if( parent->in_polygon(xx, yy) ) {
    return;
  };
  x = xx;
  y = yy;
}

bool ZOMBIE::is_contacted(int xx, int yy) {
  if( x-6 <= xx && x+6 >= xx && y-6 <= yy && y+6 >= y ) return true;
  return false;
}

//==============================================================================================================//
//  BONUS

void BONUS::paint(uint32_t *screen, int screen_width, uint32_t color, nFONT *fnt) {
  drawRectangle(screen, screen_width, x-6, y-6, 13, 13, color);
}

bool BONUS::is_contacted(int xx, int yy) {
  if( x-6 <= xx && x+6 >= xx && y-6 <= yy && y+6 >= yy ) return true;
  return false;
}

void BONUS::change_position_random(SCENE *parent) {

  int xx, yy;
  do
  {
    xx = 0;
    yy = 0;

    while( xx < 30 || xx > 800-30 ) xx = rand() % 800; 
    while( yy < 30 || yy > 800-30 ) yy = rand() % 800; 

    if( parent->in_polygon( xx, yy ) ||
        parent->in_polygon( xx-6, yy ) ||
        parent->in_polygon( xx, yy-6 ) ||
        parent->in_polygon( xx-6, yy-6 ) )
    {
       xx = 0;
       yy = 0;
    }
  }
  while( xx==0 || yy == 0 );
  
  x = xx;
  y = yy;
}


//==============================================================================================================//
//  SCENE
bool SCENE::in_polygon(int x, int y) {
  for(auto p : polygons) {
    if( p.in_polygon(x, y) ) return true;
    if( x > 1 && p.in_polygon(x-1, y) ) return true;
    if( x < 800 - 1 && p.in_polygon(x+1, y) ) return true;
    if( y > 1 && p.in_polygon(x, y-1) ) return true;
    if( y < 800 - 1 && p.in_polygon(x, y+1) ) return true;
  }
  return false;
}

void SCENE::paint(uint32_t *screen, int screen_width, nFONT *fnt) {

   Point2Di u, left, right, left_back, right_back;
   u.x = user.x;
   u.y = user.y;

   char ss[100];
   float k;
   int yy, x2, y2, mode1, mode2;
   uint32_t color;
   Point2Di left_A, left_B, right_A, right_B, P00(1, 1), P0800(1, 800-1), P800800(800-1, 800-1), P8000(800-1, 1);
   
   for(auto p : polygons) {

     p.get_left_right( &u , right, left); // найдем две максимально разнесенные точки, по которым будем рисовать тень

     calc_back_point(u, left, left_back, mode1); // найдем для точки ее проекцию на границу игрового поля
     calc_back_point(u, right, right_back, mode2); // найдем для точки ее проекцию на границу игрового поля



     Point2Di *pp[10];

     //-----------------------------------------------------------------------------//
     if( 
         ((mode1 == TOP_LEFT     || mode1 == TOP_RIGHT)    && (mode2 == TOP_LEFT     || mode2 == TOP_RIGHT))    ||
         ((mode1 == RIGHT_BOTTOM || mode1 == RIGHT_TOP)    && (mode2 == RIGHT_BOTTOM || mode2 == RIGHT_TOP))    ||
         ((mode1 == BOTTOM_LEFT  || mode1 == BOTTOM_RIGHT) && (mode2 == BOTTOM_LEFT  || mode2 == BOTTOM_RIGHT)) ||
         ((mode1 == LEFT_TOP     || mode1 == LEFT_BOTTOM)  && (mode2 == LEFT_TOP     || mode2 == LEFT_BOTTOM)) 
       ) 
     {
       pp[0] = &left_back;
       pp[1] = &left;
       pp[2] = &right;
       pp[3] = &right_back;
       

       fill_Polygon(screen, screen_width, SHADOW_COLOR, 4, pp); 
     }
     //-----------------------------------------------------------------------------//
     if( ((mode1 == TOP_LEFT || mode1 == TOP_RIGHT) && (mode2 == RIGHT_TOP || mode2 == RIGHT_BOTTOM)) ) 
     {
       pp[0] = &P8000;
       pp[1] = &left_back;
       pp[2] = &left;
       pp[3] = &right;
       pp[4] = &right_back;
       

       fill_Polygon(screen, screen_width, SHADOW_COLOR, 5, pp); 
     }
     
     if( ((mode1 == TOP_LEFT || mode1 == TOP_RIGHT) && (mode2 == BOTTOM_LEFT || mode2 == BOTTOM_RIGHT)) ) 
     {
       pp[0] = &left_back;
       pp[1] = &left;
       pp[2] = &right;
       pp[3] = &right_back;
       pp[4] = &P800800;
       pp[5] = &P8000;

       fill_Polygon(screen, screen_width, SHADOW_COLOR, 6, pp); 
     }
     
     if( ((mode1 == TOP_LEFT || mode1 == TOP_RIGHT) && (mode2 == LEFT_TOP || mode2 == LEFT_BOTTOM)) ) 
     {
       pp[0] = &P8000;
       pp[1] = &left_back;
       pp[2] = &left;
       pp[3] = &right;
       pp[4] = &right_back;
       pp[5] = &P0800;
       pp[6] = &P800800;

       fill_Polygon(screen, screen_width, SHADOW_COLOR, 7, pp); 
     }
     
     //-----------------------------------------------------------------------------//
     if( ((mode1 == RIGHT_TOP || mode1 == RIGHT_BOTTOM) && (mode2 == BOTTOM_RIGHT || mode2 == BOTTOM_LEFT)) ) 
     {
       pp[0] = &left_back;
       pp[1] = &left;
       pp[2] = &right;
       pp[3] = &right_back;
       pp[4] = &P800800;

       fill_Polygon(screen, screen_width, SHADOW_COLOR, 5, pp); 
     }
      
     if( ((mode1 == RIGHT_TOP || mode1 == RIGHT_BOTTOM) && (mode2 == LEFT_TOP || mode2 == LEFT_BOTTOM)) ) 
     {
       pp[0] = &P0800;
       pp[1] = &P800800;
       pp[2] = &left_back;
       pp[3] = &left;
       pp[4] = &right;
       pp[5] = &right_back;

       fill_Polygon(screen, screen_width, SHADOW_COLOR, 6, pp); 
     }
     
     if( ((mode1 == RIGHT_TOP || mode1 == RIGHT_BOTTOM) && (mode2 == TOP_LEFT || mode2 == TOP_RIGHT)) ) 
     {
       pp[0] = &P00;
       pp[1] = &P0800;
       pp[2] = &P800800;
       pp[3] = &left_back;
       pp[4] = &left;
       pp[5] = &right;
       pp[6] = &right_back;

       fill_Polygon(screen, screen_width, SHADOW_COLOR, 7, pp); 
     }
     
     //-----------------------------------------------------------------------------//
     if( ((mode1 == BOTTOM_RIGHT || mode1 == BOTTOM_LEFT) && (mode2 == LEFT_TOP || mode2 == LEFT_BOTTOM)) ) 
     {
       pp[0] = &P0800;
       pp[1] = &left_back;
       pp[2] = &left;
       pp[3] = &right;
       pp[4] = &right_back;

       fill_Polygon(screen, screen_width, SHADOW_COLOR, 5, pp); 
     }
     
     if( ((mode1 == BOTTOM_RIGHT || mode1 == BOTTOM_LEFT) && (mode2 == TOP_LEFT || mode2 == TOP_RIGHT)) ) 
     {
       pp[0] = &P00;
       pp[1] = &P0800;
       pp[2] = &left_back;
       pp[3] = &left;
       pp[4] = &right;
       pp[5] = &right_back;

       fill_Polygon(screen, screen_width, SHADOW_COLOR, 6, pp); 
     }
     
     if( ((mode1 == BOTTOM_RIGHT || mode1 == BOTTOM_LEFT) && (mode2 == RIGHT_BOTTOM || mode2 == RIGHT_TOP)) ) 
     {
       pp[0] = &P8000;
       pp[1] = &P00;
       pp[2] = &P0800;
       pp[3] = &left_back;
       pp[4] = &left;
       pp[5] = &right;
       pp[6] = &right_back;

       fill_Polygon(screen, screen_width, SHADOW_COLOR, 7, pp); 
     }
     
     //-----------------------------------------------------------------------------//
     if( ((mode1 == LEFT_TOP || mode1 == LEFT_BOTTOM) && (mode2 == TOP_LEFT || mode2 == TOP_RIGHT)) ) 
     {
       pp[0] = &P00;
       pp[1] = &left_back;
       pp[2] = &left;
       pp[3] = &right;
       pp[4] = &right_back;

       fill_Polygon(screen, screen_width, SHADOW_COLOR, 5, pp); 
     }
     
     if( ((mode1 == LEFT_TOP || mode1 == LEFT_BOTTOM) && (mode2 == RIGHT_TOP || mode2 == RIGHT_BOTTOM)) ) 
     {
       pp[0] = &P8000;
       pp[1] = &P00;
       pp[2] = &left_back;
       pp[3] = &left;
       pp[4] = &right;
       pp[5] = &right_back;

       fill_Polygon(screen, screen_width, SHADOW_COLOR, 6, pp); 
     }
     
     if( ((mode1 == LEFT_TOP || mode1 == LEFT_BOTTOM) && (mode2 == BOTTOM_LEFT || mode2 == BOTTOM_RIGHT)) ) 
     {
       pp[0] = &P800800;
       pp[1] = &P8000;
       pp[2] = &P00;
       pp[3] = &left_back;
       pp[4] = &left;
       pp[5] = &right;
       pp[6] = &right_back;
       pp[7] = &right_back;

       fill_Polygon(screen, screen_width, SHADOW_COLOR, 7, pp); 
     }
   }

   zombie.paint(screen, screen_width, ZOMBIE_COLOR, SCENE_COLOR, fnt);
 
   bonus.paint(screen, screen_width, BONUS_COLOR, fnt);

   user.paint(screen, screen_width, USER_COLOR, USER_IMMUNITY, fnt);   


   for(auto p : polygons) {
     p.paint(screen, screen_width, POLYGON_COLOR, fnt);
   }
}

