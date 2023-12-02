#include "mtk.hpp"
#include "font/font_img.hpp"
#include <cstring>
#include <limits>

mtk::image::image(unsigned char* mem, int w, int h) : mem(mem), w(w), h(h) {}

unsigned int mtk::image::required_size(int w, int h)
{ unsigned int result = w * h / 8;
  if ((w * h) % 8) { result++; }
  return result; }

mtk::display& mtk::image::init() { return *this; }

mtk::display& mtk::image::deinit() { return *this; }

mtk::display& mtk::image::set(int x, int y, bool val)
{ if (!mem) { return *this; }
  if (x < 0 || x >= w) { return *this; }
  if (y < 0 || y >= h) { return *this; }
  unsigned int pixel_num = y * w + x;
  unsigned int byte_num = pixel_num / 8;
  unsigned int bit_num = pixel_num % 8;
  if (val) { mem[byte_num] |=   1 << bit_num;  }
  else     { mem[byte_num] &= ~(1 << bit_num); }
  return *this; }

bool mtk::image::get(int x, int y)
{ if (!mem) { return false; }
  if (x < 0 || x >= w) { return false; }
  if (y < 0 || y >= h) { return false; }
  unsigned int pixel_num = y * w + x;
  unsigned int byte_num = pixel_num / 8;
  unsigned int bit_num = pixel_num % 8;
  return (mem[byte_num] & (1 << bit_num)); }

mtk::display& mtk::image::clear()
{ if (!mem) { return *this; }
  memset(mem, 0, required_size(w, h));
  return* this; }

mtk::display& mtk::image::refresh() { return *this; }

struct mtk::display::info mtk::image::get_info()
{ struct mtk::display::info i = { w, h };
  return i; }

mtk::part::part(mtk::display& parent, int x1, int y1, int x2, int y2)
: parent(parent), x1(x1), y1(y1), x2(x2), y2(y2) { }

mtk::display& mtk::part::init() { return *this; }

mtk::display& mtk::part::deinit() { return *this; }

mtk::display& mtk::part::set(int x, int y, bool val)
{ if (x >= 0 && x1 + x < x2 && y >= 0 && y1 + y <= y2)
  { parent.set(x1 + x, y1 + y, val); }
  return *this; }

bool mtk::part::get(int x, int y)
{ if (x >= 0 && x1 + x < x2 && y >= 0 && y1 + y <= y2)
  { return parent.get(x1 + x, y1 + y); }
  return false; }

mtk::display& mtk::part::clear()
{ for (int x = x1; x < x2; x++)
  { for (int y = y1; y < y2; y++)
    { parent.set(x, y, false); } }
  return *this; }

mtk::display& mtk::part::refresh()
{ parent.refresh();
  return *this; }

struct mtk::display::info mtk::part::get_info()
{ struct info i;
  i.w = x2 - x1;
  i.h = y2 - y1;
  return i; }

mtk::draw::draw(mtk::display& d) : d(d) { d.init(); }

mtk::draw::~draw() { d.deinit(); }

mtk::draw& mtk::draw::point(int x, int y, bool color)
{ d.set(x, y, color);
  return *this; }

mtk::draw& mtk::draw::frame(int x1, int y1, int x2, int y2, bool color)
{ for (int x = x1; x <= x2; x++) { d.set(x, y1, color).set(x, y2, color); }
  for (int y = y1; y <= y2; y++) { d.set(x1, y, color).set(x2, y, color); }
  return *this; }

mtk::draw& mtk::draw::rect(int x1, int y1, int x2, int y2, bool color)
{ for (int x = x1; x <= x2; x++)
  { for (int y = y1; y <= y2; y++)
    { d.set(x, y, color); } }
  return *this; }

mtk::draw& mtk::draw::sprite(mtk::display& img, int x, int y, bool invert)
{ for (int dx = 0; dx < img.get_info().w; dx++)
  { for (int dy = 0; dy < img.get_info().h; dy++)
    { d.set(x + dx, y + dy, invert ? !img.get(dx, dy) : img.get(dx, dy)); } }
  return *this; }

mtk::draw& mtk::draw::line(int x1, int y1, int x2, int y2, bool color)
{ int x_min = x1 < x2 ? x1 : x2;
  int x_max = x1 > x2 ? x1 : x2;
  int y_min = y1 < y2 ? y1 : y2;
  int y_max = y1 > y2 ? y1 : y2;

  if (x_min == x_max)
  { for (int y = y_min; y <= y_max; y++) { d.set(x_min, y, color); } }

  else if (y_min == y_max)
  { for (int x = x_min; x <= x_max; x++) { d.set(x, y_min, color); } }

  else
  { float k = (float)(y_max - y_min) / (float)(x_max - x_min);
    float b = (float)(x_max * y_min - x_min * y_max) / (float)(x_max - x_min);

    float k_abs = k > 0 ? k : -k;

    if (k_abs < 1)
    { for (int x = x_min; x <= x_max; x++)
      { float yf = k * x + b;
        int y = yf - (int)yf >= 0.5 ? (int)yf + 1 : (int)yf;
        d.set(x, y, color); } }
    else
    { for (int y = y_min; y <= y_max; y++)
      { float xf = ((float)y - b) / k;
        int x = xf - (int)xf >= 0.5 ? (int)xf + 1 : (int)xf;
        d.set(x, y, color); } } }

  return *this; }

mtk::draw& mtk::draw::invert(int x1, int y1, int x2, int y2)
{ for (int x = x1; x <= x2; x++)
  { for (int y = y1; y <= y2; y++)
    { d.set(x, y, !d.get(x, y)); } }
  return *this; }

static mtk::image* char_tab[256] =
/* 000 */ { &font_img::gunknown,
/* 001 */   &font_img::gunknown,
/* 002 */   &font_img::gunknown, 
/* 003 */   &font_img::gunknown, 
/* 004 */   &font_img::gunknown, 
/* 005 */   &font_img::gunknown, 
/* 006 */   &font_img::gunknown, 
/* 007 */   &font_img::gunknown, 
/* 008 */   &font_img::gunknown, 
/* 009 */   &font_img::gunknown, 
/* 010 */   &font_img::gunknown, 
/* 011 */   &font_img::gunknown, 
/* 012 */   &font_img::gunknown, 
/* 013 */   &font_img::gunknown, 
/* 014 */   &font_img::gunknown, 
/* 015 */   &font_img::gunknown, 
/* 016 */   &font_img::gunknown, 
/* 017 */   &font_img::gunknown, 
/* 018 */   &font_img::gunknown, 
/* 019 */   &font_img::gunknown, 
/* 020 */   &font_img::gunknown, 
/* 021 */   &font_img::gunknown, 
/* 022 */   &font_img::gunknown, 
/* 023 */   &font_img::gunknown, 
/* 024 */   &font_img::gunknown, 
/* 025 */   &font_img::gunknown, 
/* 026 */   &font_img::gunknown, 
/* 027 */   &font_img::gunknown, 
/* 028 */   &font_img::gunknown, 
/* 029 */   &font_img::gunknown, 
/* 030 */   &font_img::gunknown, 
/* 031 */   &font_img::gunknown, 
/* 032 */   &font_img::gspace, 
/* 033 */   &font_img::gexclamation, 
/* 034 */   &font_img::gquote, 
/* 035 */   &font_img::ghash, 
/* 036 */   &font_img::gdollar, 
/* 037 */   &font_img::gpercent, 
/* 038 */   &font_img::gand, 
/* 039 */   &font_img::gapostrophe, 
/* 040 */   &font_img::gobracket, 
/* 041 */   &font_img::gcbracket, 
/* 042 */   &font_img::gstar, 
/* 043 */   &font_img::gplus, 
/* 044 */   &font_img::gcomma, 
/* 045 */   &font_img::gminus, 
/* 046 */   &font_img::gdot, 
/* 047 */   &font_img::gslash, 
/* 048 */   &font_img::g0, 
/* 049 */   &font_img::g1, 
/* 050 */   &font_img::g2, 
/* 051 */   &font_img::g3, 
/* 052 */   &font_img::g4, 
/* 053 */   &font_img::g5, 
/* 054 */   &font_img::g6, 
/* 055 */   &font_img::g7, 
/* 056 */   &font_img::g8, 
/* 057 */   &font_img::g9, 
/* 058 */   &font_img::gcolon, 
/* 059 */   &font_img::gsemicolon, 
/* 060 */   &font_img::gless, 
/* 061 */   &font_img::gequal, 
/* 062 */   &font_img::gmore, 
/* 063 */   &font_img::gquestion, 
/* 064 */   &font_img::gat, 
/* 065 */   &font_img::gA, 
/* 066 */   &font_img::gB, 
/* 067 */   &font_img::gC, 
/* 068 */   &font_img::gD, 
/* 069 */   &font_img::gE, 
/* 070 */   &font_img::gF, 
/* 071 */   &font_img::gG, 
/* 072 */   &font_img::gH, 
/* 073 */   &font_img::gI, 
/* 074 */   &font_img::gJ, 
/* 075 */   &font_img::gK, 
/* 076 */   &font_img::gL, 
/* 077 */   &font_img::gM, 
/* 078 */   &font_img::gN, 
/* 079 */   &font_img::gO, 
/* 080 */   &font_img::gP, 
/* 081 */   &font_img::gQ, 
/* 082 */   &font_img::gR, 
/* 083 */   &font_img::gS, 
/* 084 */   &font_img::gT, 
/* 085 */   &font_img::gU, 
/* 086 */   &font_img::gV, 
/* 087 */   &font_img::gW, 
/* 088 */   &font_img::gX, 
/* 089 */   &font_img::gY, 
/* 090 */   &font_img::gZ, 
/* 091 */   &font_img::gosbracket, 
/* 092 */   &font_img::gbslash, 
/* 093 */   &font_img::gcsbracket, 
/* 094 */   &font_img::gpow, 
/* 095 */   &font_img::gunderscore, 
/* 096 */   &font_img::gunknown, 
/* 097 */   &font_img::ga, 
/* 098 */   &font_img::gb, 
/* 099 */   &font_img::gc, 
/* 100 */   &font_img::gd, 
/* 101 */   &font_img::ge, 
/* 102 */   &font_img::gf, 
/* 103 */   &font_img::gg, 
/* 104 */   &font_img::gh, 
/* 105 */   &font_img::gi, 
/* 106 */   &font_img::gj, 
/* 107 */   &font_img::gk, 
/* 108 */   &font_img::gl, 
/* 109 */   &font_img::gm, 
/* 110 */   &font_img::gn, 
/* 111 */   &font_img::go, 
/* 112 */   &font_img::gp, 
/* 113 */   &font_img::gq, 
/* 114 */   &font_img::gr, 
/* 115 */   &font_img::gs, 
/* 116 */   &font_img::gt, 
/* 117 */   &font_img::gu, 
/* 118 */   &font_img::gv, 
/* 119 */   &font_img::gw, 
/* 120 */   &font_img::gx, 
/* 121 */   &font_img::gy, 
/* 122 */   &font_img::gz, 
/* 123 */   &font_img::gocbracket, 
/* 124 */   &font_img::gunknown, 
/* 125 */   &font_img::gccbracket, 
/* 126 */   &font_img::gtilda, 
/* 127 */   &font_img::gunknown, 
/* 128 */   &font_img::gunknown, 
/* 129 */   &font_img::gunknown, 
/* 130 */   &font_img::gunknown, 
/* 131 */   &font_img::gunknown, 
/* 132 */   &font_img::gunknown, 
/* 133 */   &font_img::gunknown, 
/* 134 */   &font_img::gunknown, 
/* 135 */   &font_img::gunknown, 
/* 136 */   &font_img::gunknown, 
/* 137 */   &font_img::gunknown, 
/* 138 */   &font_img::gunknown, 
/* 139 */   &font_img::gunknown, 
/* 140 */   &font_img::gunknown, 
/* 141 */   &font_img::gunknown, 
/* 142 */   &font_img::gunknown, 
/* 143 */   &font_img::gunknown, 
/* 144 */   &font_img::gunknown, 
/* 145 */   &font_img::gunknown, 
/* 146 */   &font_img::gunknown, 
/* 147 */   &font_img::gunknown, 
/* 148 */   &font_img::gunknown, 
/* 149 */   &font_img::gunknown, 
/* 150 */   &font_img::gunknown, 
/* 151 */   &font_img::gunknown, 
/* 152 */   &font_img::gunknown, 
/* 153 */   &font_img::gunknown, 
/* 154 */   &font_img::gunknown, 
/* 155 */   &font_img::gunknown, 
/* 156 */   &font_img::gunknown, 
/* 157 */   &font_img::gunknown, 
/* 158 */   &font_img::gunknown, 
/* 159 */   &font_img::gunknown, 
/* 160 */   &font_img::gunknown, 
/* 161 */   &font_img::gunknown, 
/* 162 */   &font_img::gunknown, 
/* 163 */   &font_img::gunknown, 
/* 164 */   &font_img::gunknown, 
/* 165 */   &font_img::gunknown, 
/* 166 */   &font_img::gunknown, 
/* 167 */   &font_img::gunknown, 
/* 168 */   &font_img::gunknown, 
/* 169 */   &font_img::gunknown, 
/* 170 */   &font_img::gunknown, 
/* 171 */   &font_img::gunknown, 
/* 172 */   &font_img::gunknown, 
/* 173 */   &font_img::gunknown, 
/* 174 */   &font_img::gunknown, 
/* 175 */   &font_img::gunknown, 
/* 176 */   &font_img::gunknown, 
/* 177 */   &font_img::gunknown, 
/* 178 */   &font_img::gunknown, 
/* 179 */   &font_img::gunknown, 
/* 180 */   &font_img::gunknown, 
/* 181 */   &font_img::gunknown, 
/* 182 */   &font_img::gunknown, 
/* 183 */   &font_img::gunknown, 
/* 184 */   &font_img::gunknown, 
/* 185 */   &font_img::gunknown, 
/* 186 */   &font_img::gunknown, 
/* 187 */   &font_img::gunknown, 
/* 188 */   &font_img::gunknown, 
/* 189 */   &font_img::gunknown, 
/* 190 */   &font_img::gunknown, 
/* 191 */   &font_img::gunknown, 
/* 192 */   &font_img::gunknown, 
/* 193 */   &font_img::gunknown, 
/* 194 */   &font_img::gunknown, 
/* 195 */   &font_img::gunknown, 
/* 196 */   &font_img::gunknown, 
/* 197 */   &font_img::gunknown, 
/* 198 */   &font_img::gunknown, 
/* 199 */   &font_img::gunknown, 
/* 200 */   &font_img::gunknown, 
/* 201 */   &font_img::gunknown, 
/* 202 */   &font_img::gunknown, 
/* 203 */   &font_img::gunknown, 
/* 204 */   &font_img::gunknown, 
/* 205 */   &font_img::gunknown, 
/* 206 */   &font_img::gunknown, 
/* 207 */   &font_img::gunknown, 
/* 208 */   &font_img::gunknown, 
/* 209 */   &font_img::gunknown, 
/* 210 */   &font_img::gunknown, 
/* 211 */   &font_img::gunknown, 
/* 212 */   &font_img::gunknown, 
/* 213 */   &font_img::gunknown, 
/* 214 */   &font_img::gunknown, 
/* 215 */   &font_img::gunknown, 
/* 216 */   &font_img::gunknown, 
/* 217 */   &font_img::gunknown, 
/* 218 */   &font_img::gunknown, 
/* 219 */   &font_img::gunknown, 
/* 220 */   &font_img::gunknown, 
/* 221 */   &font_img::gunknown, 
/* 222 */   &font_img::gunknown, 
/* 223 */   &font_img::gunknown, 
/* 224 */   &font_img::gunknown, 
/* 225 */   &font_img::gunknown, 
/* 226 */   &font_img::gunknown, 
/* 227 */   &font_img::gunknown, 
/* 228 */   &font_img::gunknown, 
/* 229 */   &font_img::gunknown, 
/* 230 */   &font_img::gunknown, 
/* 231 */   &font_img::gunknown, 
/* 232 */   &font_img::gunknown, 
/* 233 */   &font_img::gunknown, 
/* 234 */   &font_img::gunknown, 
/* 235 */   &font_img::gunknown, 
/* 236 */   &font_img::gunknown, 
/* 237 */   &font_img::gunknown, 
/* 238 */   &font_img::gunknown, 
/* 239 */   &font_img::gunknown, 
/* 240 */   &font_img::gunknown, 
/* 241 */   &font_img::gunknown, 
/* 242 */   &font_img::gunknown, 
/* 243 */   &font_img::gunknown, 
/* 244 */   &font_img::gunknown, 
/* 245 */   &font_img::gunknown, 
/* 246 */   &font_img::gunknown, 
/* 247 */   &font_img::gunknown, 
/* 248 */   &font_img::gunknown, 
/* 249 */   &font_img::gunknown, 
/* 250 */   &font_img::gunknown, 
/* 251 */   &font_img::gunknown, 
/* 252 */   &font_img::gunknown, 
/* 253 */   &font_img::gunknown, 
/* 254 */   &font_img::gunknown, 
/* 255 */   &font_img::gunknown };

mtk::draw& mtk::draw::print(const char* str, int x, int y, bool invert)
{ if (!str) { return *this; }
  while (*str)
  { sprite(*char_tab[*(unsigned char*)str], x, y, invert);
    x += char_tab[*(unsigned char*)str]->get_info().w; str++; }
  return *this; }

mtk::draw& mtk::draw::refresh() { d.refresh(); return *this; }

mtk::draw& mtk::draw::clear() { d.clear(); return *this; }

mtk::widget::widget(const char* text, int x1, int y1, int x2, int y2)
: text(text), x1(x1), y1(y1), x2(x2), y2(y2), accept_focus(false),
  focused(false), next(nullptr) { }

mtk::widget& mtk::widget::geometry(int x1, int y1, int x2, int y2)
{ this->x1 = x1; this->y1 = y1;
  this->x2 = x2; this->y2 = y2;
  return *this; }

mtk::widget& mtk::widget::label_text(const char* text)
{ this->text = text;
  return *this; }

mtk::screen::screen(const char* title) : title(title), w(nullptr) {}

mtk::screen& mtk::screen::add_widget(mtk::widget* widget)
{ if (!w) { w = widget; return *this; }
  mtk::widget* last = w;
  while (last->next) { last = last->next; }
  last->next = widget;
  return *this; }

mtk::label::label(const char* text, int x1, int y1, int x2, int y2)
: mtk::widget(text, x1, y1, x2, y2) { }

void mtk::label::event(mtk::event::base* ev)
{ switch (ev->t)
  { case mtk::event::base::type::draw:
    { mtk::event::draw* dev = (mtk::event::draw*)ev;
      mtk::draw drawer(*dev->area);
      drawer.print(text, 0, 0);
    } break;

    default: break; } }

mtk::sprite::sprite(mtk::image* img, int x1, int y1, int x2, int y2)
: mtk::widget(nullptr, x1, y1, x2, y2), img(img) { }

mtk::sprite& mtk::sprite::set_image(mtk::image* img)
{ this->img = img; return *this; }

void mtk::sprite::event(mtk::event::base* ev)
{ switch (ev->t)
  { case mtk::event::base::type::draw:
    { mtk::event::draw* dev = (mtk::event::draw*)ev;
      mtk::draw drawer(*dev->area);
      drawer.sprite(*img, 0, 0);
    } break;

    default: break; } }

mtk::button::button(const char* text, void* arg, void (*callback)(void*),
                    int x1, int y1, int x2, int y2)
: mtk::widget(text, x1, y1, x2, y2), callback(callback), arg(arg)
{ accept_focus = true; }

mtk::button& mtk::button::set_callback(void* arg, void (*callback)(void*))
{ this->arg = arg; this->callback = callback; return *this; }

void mtk::button::event(mtk::event::base* ev)
{ switch (ev->t)
  { case mtk::event::base::type::draw:
    { mtk::event::draw* dev = (mtk::event::draw*)ev;
      mtk::draw drawer(*dev->area);
      drawer.clear();
      if (focused)
      { drawer.rect(0, 0,
                    dev->area->get_info().w - 1,
                    dev->area->get_info().h - 1)
              .print(text, 2, 2, true); }
      else
      { drawer.print(text, 2, 2, false)
              .frame(0, 0,
                     dev->area->get_info().w - 1, 
                     dev->area->get_info().h - 1); }
    } break;

    case mtk::event::base::type::trigger:
    { if (callback) { callback(arg); }
    } break;

    default: break; } }

static int distance_rating(int x1, int y1, int x2, int y2)
{ int w = x1 > x2 ? x1 - x2 : x2 - x1;
  int h = y1 > y2 ? y1 - y2 : y2 - y1;
  return w + h; }

mtk::core::core(mtk::display& d) : s(nullptr), d(d) { }

mtk::core& mtk::core::show(mtk::screen* scr)
{ if (!scr) { return *this; }
  draw drawer(d);
  drawer.clear()
        .rect(0, 0, d.get_info().w, 8)
        .print(scr->title, 1, 1, true);
  s = scr;
  if (!focused()) { focus_first(); }
  widget* w = scr->w; while (w) { draw_widget(w); w = w->next; }
  drawer.refresh();
  return *this; }

mtk::core& mtk::core::click(mtk::core::sysclick code)
{ if (!s) { return *this; }

  if (code == sysclick::ok)
  { widget* focus = focused();
    if (!focus) { return *this; }
    mtk::event::trigger ev;
    focus->event(&ev); }
  else if (code == sysclick::up   || code == sysclick::down ||
           code == sysclick::left || code == sysclick::right)
  { if (!s->w) { return *this; }

     widget* focus = focused();
     if (!focus) { draw_widget(focus_first()); return *this; }

     direction dir = direction::up;
     switch (code)
     { case sysclick::up:    dir = direction::up;    break;
       case sysclick::down:  dir = direction::down;  break;
       case sysclick::left:  dir = direction::left;  break;
       case sysclick::right: dir = direction::right; break;
       default: return *this; }

     widget* target = find_nearest(focus, dir);

     if (!target) { return *this; }

     target->focused = true;
     focus->focused = false;

     draw_widget(target);
     draw_widget(focus);
  }

  return *this; }

mtk::display::info mtk::core::dinfo()
{ struct display::info info = d.get_info();
  info.h -= 10;
  return info; }

mtk::widget* mtk::core::focused()
{ if (!s) { return nullptr; }
  mtk::widget* root = s->w;
  while (root)
  { if (root->focused) { return root; }
    root = root->next; }
  return nullptr; }

mtk::widget* mtk::core::focus_first()
{ widget* iter = s->w;
  while (iter)
  { if (!iter->accept_focus) { iter = iter->next; continue; }
    iter->focused = true; return iter; }
  return nullptr; }

void mtk::core::draw_widget(mtk::widget* w)
{ if (!w) { return; }
  part p(d, w->x1, w->y1 + 10, w->x2, w->y2 + 10);
  event::draw ev(&p);
  w->event(&ev);
  d.refresh(); }

mtk::widget* mtk::core::find_nearest(mtk::widget* tw, mtk::core::direction dir)
{ if (!tw || !s || !s->w) { return nullptr; }

  widget* nearest = nullptr;

  int rating = std::numeric_limits<int>::max();
  widget* iter = s->w;
  int tx = (tw->x1 + tw->x2) / 2; int ty = (tw->y1 + tw->y2) / 2;

  while (iter)
  { if (iter == tw) { iter = iter->next; continue; }
    if (!iter->accept_focus) { iter = iter->next; continue; }

    int cx = (iter->x1 + iter->x2) / 2; int cy = (iter->y1 + iter->y2) / 2;

    if (dir == direction::up    && cy >= ty) { iter = iter->next; continue; }
    if (dir == direction::down  && cy <= ty) { iter = iter->next; continue; }
    if (dir == direction::left  && cx >= tx) { iter = iter->next; continue; }
    if (dir == direction::right && cx <= tx) { iter = iter->next; continue; }

    int current_rating = distance_rating(cx, cy, tx, ty);

    if (current_rating < rating) { nearest = iter; rating = current_rating; }

    iter = iter->next; }

  return nearest; }
