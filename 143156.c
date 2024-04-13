    template<typename tc>
    CImg<T>& draw_text(const int x0, const int y0,
                       const char *const text,
                       const int, const tc *const background_color,
                       const float opacity=1, const unsigned int font_height=13, ...) {
      if (!font_height) return *this;
      CImg<charT> tmp(2048);
      std::va_list ap; va_start(ap,font_height);
      cimg_vsnprintf(tmp,tmp._width,text,ap); va_end(ap);
      return draw_text(x0,y0,"%s",(tc*)0,background_color,opacity,font_height,tmp._data);