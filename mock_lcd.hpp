#ifndef MOCK_LCD_HPP
#define MOCK_LCD_HPP

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>
#include "mtk.hpp"

class mock_lcd : public Fl_Widget, public mtk::display
{ public:
  enum class palette { lcd_green,
                       lcd_yellow,
                       oled_blue,
                       oled_white,
                       oled_green,
                       plain_bw };

  mock_lcd(int w, int h, palette colors, int gridw, int pixelw);
  virtual mtk::display& init() override;
  virtual mtk::display& deinit() override;
  virtual mtk::display& set(int x, int y, bool val) override;
  virtual bool get(int x, int y) override;
  virtual mtk::display& clear() override;
  virtual mtk::display& refresh() override;
  virtual struct mtk::display::info get_info() override;

  private:
  virtual void draw() override;
  int pixw, pixh, gridw, pixelw;
  bool* vmem;
  palette colors; };

#endif // MOCK_LCD_HPP
