#include <stdio.h>
#include <vector>
#include <cmath>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <time.h>

#include "font.h"
#include "primitives.h"
#include "scene.h"

SCENE scene;

nFONT font;

int key_press_A = 0
  , key_press_S = 0
  , key_press_D = 0
  , key_press_W = 0;
int mouse_x = 0, mouse_y = 0;

uint32_t screen[800*800];

void Copy_ToCanvas(uint32_t* ptr, int w, int h) {
  EM_ASM_({
      let data = Module.HEAPU8.slice($0, $0 + $1 * $2 * 4);
      let context = Module['canvas'].getContext('2d');
      let imageData = context.getImageData(0, 0, $1, $2);
      imageData.data.set(data);
      context.putImageData(imageData, 0, 0);
    }, ptr, w, h);
}

//int vv = 0;

static void main_loop()
{
   uint32_t *q, col;;
   q = screen;
   col = scene.SCENE_COLOR;
   for(int i=0;i<800*800;i++) *q++ = col;
  
   if( scene.user.is_immunity > 0 ) scene.user.is_immunity--;

   bool flag_x = false, flag_y = false;
   if( key_press_D == 1 && scene.user.x < 750) { scene.user.speed_x_plus();  flag_x = true; };
   if( key_press_A == 1 && scene.user.x >  50) { scene.user.speed_x_minus(); flag_x = true; };
   if( key_press_S == 1 && scene.user.y < 750) { scene.user.speed_y_plus();  flag_y = true; };
   if( key_press_W == 1 && scene.user.y >  50) { scene.user.speed_y_minus(); flag_y = true; };
   
   if( flag_x == false ) {
     if(scene.user.speed_x > 0) scene.user.speed_x -= 0.45;
     if(scene.user.speed_x < 0) scene.user.speed_x += 0.45;
   }
   if( flag_y == false ) {
     if(scene.user.speed_y > 0) scene.user.speed_y -= 0.45;
     if(scene.user.speed_y < 0) scene.user.speed_y += 0.45;
   }

   scene.paint(screen, 800, &font);

   scene.zombie.calc_move(screen, 800, &scene);

   scene.user.calc_move(&scene);

   char s[100];

   sprintf(s, "score: %d", scene.user.score);
   font.paint(screen, 800, 400-40, 15, 0xffffffff, s);   


   sprintf(s, "mouse %03d:%03d", mouse_x, mouse_y);
   font.paint(screen, 800, 670, 15, 0xff00aa00, s);   

   sprintf(s, "speed %.2f:%.2f", scene.user.speed_x, scene.user.speed_y);
   font.paint(screen, 800, 670, 35, 0xff00aa00, s);

   sprintf(s, "user %03d:%03d", scene.user.x, scene.user.y);
   font.paint(screen, 800, 670, 55, 0xff00aa00, s);

   Copy_ToCanvas(screen, 800, 800);
}

void key_press(char key) {
  if(key == 'a') key_press_A = 1;
  if(key == 's') key_press_S = 1;
  if(key == 'd') key_press_D = 1;
  if(key == 'w') key_press_W = 1;
};

void key_unpress(char key) {
  if(key == 'a') key_press_A = 0;
  if(key == 's') key_press_S = 0;
  if(key == 'd') key_press_D = 0;
  if(key == 'w') key_press_W = 0;
};

EM_BOOL key_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData)
{
  if( e->repeat == 1 && ( e->keyCode == 87 || // w
                          e->keyCode == 65 || // a
                          e->keyCode == 68 || // d
                          e->keyCode == 83    // s
                        ) ) 
  { return 0; }

  if( eventType == 2 && e->repeat == 0 && e->keyCode == 87) { key_press('w'); return 0; };
  if( eventType == 2 && e->repeat == 0 && e->keyCode == 65) { key_press('a'); return 0; };
  if( eventType == 2 && e->repeat == 0 && e->keyCode == 68) { key_press('d'); return 0; };
  if( eventType == 2 && e->repeat == 0 && e->keyCode == 83) { key_press('s'); return 0; };

  if( eventType == 3 && e->repeat == 0 && e->keyCode == 87) { key_unpress('w'); return 0; };
  if( eventType == 3 && e->repeat == 0 && e->keyCode == 65) { key_unpress('a'); return 0; };
  if( eventType == 3 && e->repeat == 0 && e->keyCode == 68) { key_unpress('d'); return 0; };
  if( eventType == 3 && e->repeat == 0 && e->keyCode == 83) { key_unpress('s'); return 0; };

  return 0;
}

EM_BOOL mouse_callback(int eventType, const EmscriptenMouseEvent *e, void *userData)
{
  mouse_x = e->targetX;
  mouse_y = e->targetY;
  return 0;
}


int main()
{

    srand(time(NULL));

    font.init();
    scene.first_init();

    EMSCRIPTEN_RESULT ret;

    ret = emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, 0, 1, key_callback);
    ret = emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, 0, 1, key_callback);
    ret = emscripten_set_mousemove_callback("#canvas", 0, 1, mouse_callback);

    EMSCRIPTEN_RESULT r = emscripten_set_canvas_element_size("#canvas", 800, 800);
    emscripten_set_main_loop(main_loop, 300, 0);

    return 0;
}