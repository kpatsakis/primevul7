    template<typename tc, typename t>
    CImg<T>& draw_text(const int x0, const int y0,
                       const char *const text,
                       const int, const tc *const background_color,
                       const float opacity, const CImgList<t>& font, ...) {
      if (!font) return *this;
      CImg<charT> tmp(2048);
      std::va_list ap; va_start(ap,font);
      cimg_vsnprintf(tmp,tmp._width,text,ap); va_end(ap);
      return _draw_text(x0,y0,tmp,(tc*)0,background_color,opacity,font,false);