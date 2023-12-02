#include "mtk.hpp"
#include "mock_lcd.hpp"
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <iostream>

class app
{ public:

  app(mtk::core& tk) : tk(tk), labels_screen(tk.dinfo()) { }

  class labels_screen : public mtk::screen
  { public:
    labels_screen(struct mtk::display::info dinfo)
    : mtk::screen("labels example")
    { label_0.label_text("sample label 1").geometry( 0,  0,  60,  8);
      label_1.label_text("sample label 2").geometry( 0,  8,  60, 16);
      label_2.label_text("sample label 3").geometry(60,  0, 120,  8);

      next_button.label_text("next")
                 .geometry(dinfo.w - 40, dinfo.h - 11, dinfo.w, dinfo.h);
      
      add_widget(&label_0).add_widget(&label_1).add_widget(&label_2)
      .add_widget(&next_button); }

    mtk::label label_0;
    mtk::label label_1;
    mtk::label label_2;
    mtk::button next_button;
  } labels_screen;

  mtk::core& tk; };

class main_window : public Fl_Window
{ public:
  explicit main_window()
  : Fl_Window(1036, 534, "mock lcd example with monochrome toolkit"),
    lcd(128, 64, mock_lcd::palette::lcd_green, 1, 6),
    tk(lcd), a(tk),
    up((lcd.w() / 5) * 0, lcd.h() + 10, lcd.w() / 5, 20, "up"),
    dn((lcd.w() / 5) * 1, lcd.h() + 10, lcd.w() / 5, 20, "dn"),
    lt((lcd.w() / 5) * 2, lcd.h() + 10, lcd.w() / 5, 20, "lt"),
    rt((lcd.w() / 5) * 3, lcd.h() + 10, lcd.w() / 5, 20, "rt"),
    ok((lcd.w() / 5) * 4, lcd.h() + 10, lcd.w() / 5, 20, "ok")
  { resize(x(), y(), lcd.w(), lcd.h() + 30);
    up.callback(up_cb, &tk); up.shortcut('w');
    dn.callback(dn_cb, &tk); dn.shortcut('s');
    lt.callback(lt_cb, &tk); lt.shortcut('a');
    rt.callback(rt_cb, &tk); rt.shortcut('d');
    ok.callback(ok_cb, &tk); ok.shortcut(FL_Enter);
    tk.show(&a.labels_screen); }

  mock_lcd lcd;
  mtk::core tk;
  app a;
  Fl_Button up;
  Fl_Button dn;
  Fl_Button lt;
  Fl_Button rt;
  Fl_Button ok;

  static void up_cb(Fl_Widget* w, void* arg)
  { mtk::core* tk = (mtk::core*)arg;
    tk->click(mtk::core::sysclick::up); }

  static void dn_cb(Fl_Widget* w, void* arg)
  { mtk::core* tk = (mtk::core*)arg;
    tk->click(mtk::core::sysclick::down); }

  static void lt_cb(Fl_Widget* w, void* arg)
  { mtk::core* tk = (mtk::core*)arg;
    tk->click(mtk::core::sysclick::left); }

  static void rt_cb(Fl_Widget* w, void* arg)
  { mtk::core* tk = (mtk::core*)arg;
    tk->click(mtk::core::sysclick::right); }

  static void ok_cb(Fl_Widget* w, void* arg)
  { mtk::core* tk = (mtk::core*)arg;
    tk->click(mtk::core::sysclick::ok); } };

int main(int argc, char** argv)
{ main_window w;
  w.show();
  Fl::run();
  return 0; }
