#include "mock_lcd.hpp"
#include <cstdint>

typedef struct
{ uint8_t backgroung[3];
  uint8_t active_pixel[3];
  uint8_t inactive_pixel[3];
} color_set;

static color_set lcd_green  = { {0x00, 0xaa, 0x00},
                                {0x00, 0x00, 0x00},
                                {0x00, 0x80, 0x00} };

static color_set lcd_yellow = { {0xaa, 0xaa, 0x00},
                                {0x00, 0x00, 0x00},
                                {0x80, 0x80, 0x00} };

static color_set oled_blue  = { {0x00, 0x00, 0x00},
                                {0x00, 0xff, 0xff},
                                {0x00, 0x00, 0x00} };

static color_set oled_white = { {0x00, 0x00, 0x00},
                                {0xff, 0xff, 0xff},
                                {0x00, 0x00, 0x00} };

static color_set oled_green = { {0x00, 0x00, 0x00},
                                {0x00, 0xff, 0x00},
                                {0x00, 0x00, 0x00} };

static color_set plain_bw   = { {0xff, 0xff, 0xff},
                                {0x00, 0x00, 0x00},
                                {0xff, 0xff, 0xff} };

static color_set get_colors(mock_lcd::palette colors)
{ color_set set = { {0x00, 0x00, 0x00},
                    {0x00, 0x00, 0x00},
                    {0x00, 0x00, 0x00} };

  switch (colors)
  { case mock_lcd::palette::lcd_green:  set = lcd_green;  break;
    case mock_lcd::palette::lcd_yellow: set = lcd_yellow; break;
    case mock_lcd::palette::oled_blue:  set = oled_blue;  break;
    case mock_lcd::palette::oled_white: set = oled_white; break;
    case mock_lcd::palette::oled_green: set = oled_green; break;
    case mock_lcd::palette::plain_bw:   set = plain_bw;   break;
    default: break; }

  return set; }

mock_lcd::mock_lcd(int w, int h, mock_lcd::palette colors,
                   int gridw, int pixelw)
: Fl_Widget(0, 0, 0, 0), pixw(w), pixh(h), colors(colors), gridw(gridw),
  pixelw(pixelw)
{ this->w(w * (gridw + pixelw) + gridw);
  this->h(h * (gridw + pixelw) + gridw);
  vmem = new bool[w * h]; }

mtk::display& mock_lcd::init() { return *this; }

mtk::display& mock_lcd::deinit() { return *this; }

mtk::display& mock_lcd::set(int x, int y, bool val)
{ if (!vmem) { return *this; }
  if (x < 0 || x >= pixw) { return *this; }
  if (y < 0 || y >= pixh) { return *this; }
  vmem[y * pixw + x] = val;
  return *this; }

bool mock_lcd::get(int x, int y)
{ if (!vmem) { return false; }
  if (x < 0 || x >= pixw) { return false; }
  if (y < 0 || y >= pixh) { return false; }
  return vmem[y * pixw + x]; }

mtk::display& mock_lcd::clear()
{ if (!vmem) { return *this; }
  memset(vmem, 0, pixw * pixh);
  return *this; }

mtk::display& mock_lcd::refresh() { redraw(); return *this; }

struct mtk::display::info mock_lcd::get_info()
{ struct info i = { pixw, pixh };
  return i; }

void mock_lcd::draw()
{ fl_rectf(x(), y(), w(), h(), fl_rgb_color(get_colors(colors).backgroung[0],
                                            get_colors(colors).backgroung[1],
                                            get_colors(colors).backgroung[2]));
  if (!vmem) { return; }

  for (int pixy = 0; pixy < pixh; pixy++)
  { for (int pixx = 0; pixx < pixw; pixx++)
    { fl_rectf(gridw + pixx * (gridw + pixelw),
               gridw + pixy * (gridw + pixelw),
               pixelw, pixelw,
               get(pixx, pixy)
               ? fl_rgb_color(get_colors(colors).active_pixel[0],
                              get_colors(colors).active_pixel[1],
                              get_colors(colors).active_pixel[2])
               : fl_rgb_color(get_colors(colors).inactive_pixel[0],
                              get_colors(colors).inactive_pixel[1],
                              get_colors(colors).inactive_pixel[2])); } } }
