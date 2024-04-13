    template<typename tc, typename t>
    CImg<T>& draw_text(const int x0, const int y0,
                       const char *const text,
                       const tc *const foreground_color, const int,
                       const float opacity, const CImgList<t>& font, ...) {
      if (!font) return *this;
      CImg<charT> tmp(2048);
      std::va_list ap; va_start(ap,font);
      cimg_vsnprintf(tmp,tmp._width,text,ap); va_end(ap);
      return _draw_text(x0,y0,tmp,foreground_color,(tc*)0,opacity,font,false);