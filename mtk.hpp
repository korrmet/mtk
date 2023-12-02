#ifndef MTK_HPP
#define MTK_HPP

namespace mtk {
class display
{ public:
  struct info { int w; int h; };
  virtual display& init() = 0;
  virtual display& deinit() = 0;
  virtual display& set(int x, int y, bool val) = 0;
  virtual bool get(int x, int y) = 0;
  virtual display& clear() = 0;
  virtual display& refresh() = 0;
  virtual struct info get_info() = 0; };

class image : public display
{ public:
  image(unsigned char* mem, int w, int h);
  static unsigned int required_size(int w, int h);

  virtual display& init() override;
  virtual display& deinit() override;
  virtual display& set(int x, int y, bool val) override;
  virtual bool get(int x, int y) override;
  virtual display& clear() override;
  virtual display& refresh() override;
  virtual struct display::info get_info() override;

  private:
  unsigned char* mem;
  int w, h; };

class part : public display
{ public:
  part(display& parent, int x1, int y1, int x2, int y2);

  virtual display& init() override;
  virtual display& deinit() override;
  virtual display& set(int x, int y, bool val = true) override;
  virtual bool get(int x, int y) override;
  virtual display& clear() override;
  virtual display& refresh() override;
  virtual struct display::info get_info() override;

  private:
  display& parent;
  int x1, y1, x2, y2; };

class draw
{ public:
  draw(display& d);
  ~draw();

  draw& point(int x, int y, bool color = true);
  draw& frame(int x1, int y1, int x2, int y2, bool color = true);
  draw& rect(int x1, int y1, int x2, int y2, bool color = true);
  draw& sprite(display& img, int x, int y, bool invert = false);
  draw& line(int x1, int y1, int x2, int y2, bool color = true);
  draw& invert(int x1, int y1, int x2, int y2);
  draw& print(const char* str, int x, int y, bool invert = false);
  draw& refresh();
  draw& clear();

  private:
  display& d; };

namespace event {
class base
{ public: 
  enum class type { draw, trigger } t;
  base(type t) : t(t) {} };

struct draw : public base
{ public:
  draw(display* area = nullptr) : base(base::type::draw), area(area) {}
  display* area; };

struct trigger : public base
{ public:
  trigger() : base(base::type::trigger) {} };
}

class widget
{ public:
  widget(const char* text = nullptr,
         int x1 = 0, int y1 = 0, int x2 = 0, int y2 = 0);
  widget& geometry(int x1, int y1, int x2, int y2);
  widget& label_text(const char* text);
  virtual void event(event::base* ev) = 0;

  protected:
  const char* text;
  int x1, y1, x2, y2;
  bool accept_focus, focused;

  private:
  friend class screen;
  friend class core;
  widget* next; };

class screen
{ public:
  screen(const char* title);
  screen& add_widget(widget* w);

  private:
  friend class core;
  const char* title;
  widget* w; };

class label : public widget
{ public:
  label(const char* text = nullptr,
        int x1 = 0, int y1 = 0, int x2 = 0, int y2 = 0);
  virtual void event(event::base* ev) override; };

class sprite : public widget
{ public:
  sprite(image* img = nullptr,
         int x1 = 0, int y1 = 0, int x2 = 0, int y2 = 0);
  sprite& set_image(image* img);
  virtual void event(event::base* ev) override;

  private:
  image* img; };

class button : public widget
{ public:
  button(const char* text = nullptr,
         void* arg = nullptr, void (*callback)(void*) = nullptr,
         int x1 = 0, int y1 = 0, int x2 = 0, int y2 = 0);
  button& set_callback(void* arg, void (*callback)(void*));
  virtual void event(event::base* ev) override;

  private:
  void (*callback)(void*);
  void* arg; };

class core
{ public:
  enum class sysclick { up, down, left, right, ok };
  core(display& d);
  core& show(screen* scr);
  core& click(sysclick code);
  struct display::info dinfo();

  private:
  mtk::widget* focused();
  mtk::widget* focus_first();
  void draw_widget(mtk::widget* w);
  enum class direction { up, down, left, right };
  mtk::widget* find_nearest(mtk::widget* tw, direction dir);
  screen* s;
  display& d; };
}

#endif // MTK_HPP
