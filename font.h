#pragma once

class nBUKVA
{
public:
  int idx = -1, w = 0, h = 0;
  unsigned char *buf = nullptr;

  int paint(uint32_t *screen, int screen_width, int x, int y, uint32_t color);
  void set_len(int start, int len, int pix);
  void set(int h_, int k_, int val);
  void init(int w_, int h_, int idx_);
  
  nBUKVA() { };
};


class nFONT
{
  public:
  nBUKVA bukva[256];
  void paint(uint32_t *screen, int screen_width, int x, int y, uint32_t color, char *txt);
  bool  set_from_buffer_LZ(uint16_t *buf);

  void init();

  nFONT()
  {

  };
};

