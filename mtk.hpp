#ifndef MTK_HPP
#define MTK_HPP

namespace mtk {

/** \brief basic class of the monochrome display */
class display
{ public:

  /** \brief information about display */
  struct info
  { /** \brief width of the display in pixels  */
    int w;

    /** \brief height of the display in pixels */
    int h; };

  /** \brief initializes display */
  virtual display& init() = 0;

  /** \brief deinitializes display */
  virtual display& deinit() = 0;

  /** \brief set point with coordinates to specific value
   *  \param x horizontal distance from the left side
   *  \param y vertical distacne from the top
   *  \param val value of the point */
  virtual display& set(int x, int y, bool val) = 0;

  /** \brief get value of the point at the specifiet coordinate
   *  \param x horizontal distance from the top left side
   *  \param y vertical distance from the top */
  virtual bool get(int x, int y) = 0;

  /** \brief sets all of the points to the 0 */
  virtual display& clear() = 0;

  /** \brief refresh the screen / apply the video memory to the screen */
  virtual display& refresh() = 0;

  /** \brief returns information about display */
  virtual struct info get_info() = 0; };

/** \brief monochrome image */
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

/** \brief part of the another display, maps the rectancle area of a display
 *         object to 0:0 coordinates with selected width and height
 *  \details used to draw objects in a safe way, because all of the drawing
 *           primitives don't be drawn outside of the selected part */
class part : public display
{ public:
  /** \brief constructor
   *  \param parent reference to the display, that should be mapped to this part
   *  \param x1 horizontal distance from the left side to the top left corner of
   *            the part's frame
   *  \param y1 vertical distance from the top to the top left corner of the
   *            part's frame
   *  \param x2 horizontal distance from the left side to the bottom right
   *            corner of the part's frame
   *  \param y2 vertical distance from the top to the bottom right corner of the
   *            part's frame */
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

/** \brief set of the drawing primitives */
class draw
{ public:
  /** \brief constructor
   *  \param d reference to the display object to draw */
  draw(display& d);
  ~draw();

  /** \brief draw point, just set's the point at coordinates to the specified
   *         value
   *  \param x horizontal distance form the left side of the display to point
   *  \param y vertical distance from the top of the display to point
   *  \param color value of the point */
  draw& point(int x, int y, bool color = true);

  /** \brief draw the rectangular frame at the selected coordinates
   *  \param x1 horiziontal distance from the left side of the display to the
   *            top left point of the frame
   *  \param y1 vertical distance from the top of the display to the top left
   *            point of the frame
   *  \param x2 horizontal distance from the left side of the display to the
   *            bottom right point of the frame
   *  \param y2 vertical distance from the top of the display to the bottom
   *            right point of the frame
   *  \param color value of the frame's points */
  draw& frame(int x1, int y1, int x2, int y2, bool color = true);

  /** \brief draw the filled rectangle at the selected coordinates
   *  \param x1 horiziontal distance from the left side of the display to the
   *            top left point of the rectangle
   *  \param y1 vertical distance from the top of the display to the top left
   *            point of the rectangle
   *  \param x2 horizontal distance from the left side of the display to the
   *            bottom right point of the rectangle
   *  \param y2 vertical distance from the top of the display to the bottom
   *            right point of the rectangle
   *  \param color value of the rectangle's points */
  draw& rect(int x1, int y1, int x2, int y2, bool color = true);

  /** \brief draws image from the display object at the specified coordinates
   *  \details copy all of the points of image to the current display
   *  \param img display object that would be used as image source
   *  \param x horiziontal distance from the left side of the current display
   *           to the top left corner of the draw area
   *  \param y vertical distance from the top side of the current diplay to the
   *           top left corner of the draw area
   *  \param invert if this option is true image points values would be
   *                inverted */
  draw& sprite(display& img, int x, int y, bool invert = false);

  /** \brief draws line with the specified coordinates */
  draw& line(int x1, int y1, int x2, int y2, bool color = true);

  /** \brief inverts the existing image at the coordinates */
  draw& invert(int x1, int y1, int x2, int y2);

  /** \brief prints c-string at the selected coordinates */
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
