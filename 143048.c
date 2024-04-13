    template<typename tc1, typename tc2>
    CImg<T>& draw_text(const int x0, const int y0,
                       const char *const text,
                       const tc1 *const foreground_color, const tc2 *const background_color,
                       const float opacity=1, const unsigned int font_height=13, ...) {
      if (!font_height) return *this;
      CImg<charT> tmp(2048);
      std::va_list ap; va_start(ap,font_height);
      cimg_vsnprintf(tmp,tmp._width,text,ap); va_end(ap);
      const CImgList<ucharT>& font = CImgList<ucharT>::font(font_height,true);
      _draw_text(x0,y0,tmp,foreground_color,background_color,opacity,font,true);
      return *this;