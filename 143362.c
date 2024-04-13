    template<typename tc>
    CImg<T>& draw_text(const int x0, const int y0,
                       const char *const text,
                       const tc *const foreground_color, const int background_color=0,
                       const float opacity=1, const unsigned int font_height=13, ...) {
      if (!font_height) return *this;
      cimg::unused(background_color);
      CImg<charT> tmp(2048);
      std::va_list ap; va_start(ap,font_height);
      cimg_vsnprintf(tmp,tmp._width,text,ap); va_end(ap);
      return draw_text(x0,y0,"%s",foreground_color,(const tc*)0,opacity,font_height,tmp._data);