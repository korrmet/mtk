#include "mtk.hpp"
#include "mock_lcd.hpp"
#include "images/images_img.hpp"
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <iostream>

class app
{ public:

  app(mtk::core& tk)
  : tk(tk),
    labels_screen(tk.dinfo(), this),
    buttons_screen(tk.dinfo(), this),
    images_screen(tk.dinfo(), this)
  { }

  class labels_screen : public mtk::screen
  { public:
    labels_screen(struct mtk::display::info dinfo, app* application)
    : mtk::screen("Labels (example) [# 1/3]")
    { label_0.label_text("sample label 1").geometry( 0,  0,  60,  8);
      label_1.label_text("sample label 2").geometry( 0,  8,  60, 16);
      label_2.label_text("sample label 3").geometry(60,  0, 120,  8);

      next_button.set_callback(application, next_handler)
                 .label_text("next")
                 .geometry(dinfo.w - 40, dinfo.h - 11, dinfo.w, dinfo.h);
      
      add_widget(&label_0).add_widget(&label_1).add_widget(&label_2)
      .add_widget(&next_button); }

    mtk::label label_0, label_1, label_2;
    mtk::button next_button;

    static void next_handler(void* app_ptr)
    { app* that = (app*)app_ptr;
      that->tk.show(&that->buttons_screen); }
  } labels_screen;

  class buttons_screen : public mtk::screen
  { public:
    buttons_screen(struct mtk::display::info dinfo, app* application)
    : mtk::screen("Buttons (example) [# 2/3]")
    { next_button.set_callback(application, next_handler)
                 .label_text("next")
                 .geometry(dinfo.w - 40, dinfo.h - 11, dinfo.w, dinfo.h);
      b1.label_text("1").geometry( 0,  0, 11, 11);
      b2.label_text("2").geometry(12,  0, 23, 11);
      b3.label_text("3").geometry(24,  0, 35, 11);
      b4.label_text("4").geometry( 0, 12, 11, 23);
      b5.label_text("5").geometry(12, 12, 23, 23);
      b6.label_text("6").geometry(24, 12, 35, 23);
      b7.label_text("7").geometry( 0, 24, 11, 35);
      b8.label_text("8").geometry(12, 24, 23, 35);
      b9.label_text("9").geometry(24, 24, 35, 35);
      b0.label_text("0").geometry(12, 36, 23, 47);
      add_widget(&next_button)
        .add_widget(&b0).add_widget(&b1).add_widget(&b2)
        .add_widget(&b3).add_widget(&b4).add_widget(&b5)
        .add_widget(&b6).add_widget(&b7).add_widget(&b8)
        .add_widget(&b9); }

    mtk::button b0, b1, b2, b3, b4, b5, b6, b7, b8, b9;
    mtk::button next_button;

    static void next_handler(void* app_ptr)
    { app* that = (app*)app_ptr;
      that->tk.show(&that->images_screen); }
  } buttons_screen;

  class images_screen : public mtk::screen
  { public:
    images_screen(struct mtk::display::info dinfo, app* application)
    : mtk::screen("Images (example) [# 3/3]")
    { first_button.set_callback(application, first_handler)
                  .label_text("first")
                  .geometry(dinfo.w - 40, dinfo.h - 11, dinfo.w, dinfo.h);

      p0.set_image(&images_img::logo).geometry( 5, 5, 31, 14);

      add_widget(&first_button).add_widget(&p0); }

    mtk::button first_button;
    mtk::sprite p0;

    static void first_handler(void* app_ptr)
    { app* that = (app*)app_ptr;
      that->tk.show(&that->labels_screen); }
  } images_screen;

  mtk::core& tk; };

class main_window : public Fl_Window
{ public:
  explicit main_window()
  : Fl_Window(1036, 534, "mock lcd example with monochrome toolkit"),
    lcd(128, 64, mock_lcd::palette::oled_blue, 1, 6),
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
