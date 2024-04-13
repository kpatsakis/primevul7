    // [internal] Version used to display text in interactive viewers.
    CImg<T>& __draw_text(const char *const text, const int is_down, ...) {
      CImg<charT> tmp(2048);
      std::va_list ap; va_start(ap,is_down);
      cimg_vsnprintf(tmp,tmp._width,text,ap); va_end(ap);
      CImg<ucharT> a_label, a_labelmask;
      const unsigned char a_labelcolor = 255;
      const unsigned int fsize = 14;
      a_label.draw_text(0,0,"%s",&a_labelcolor,0,1,fsize,tmp._data).normalize(0,255);
      if (a_label) {
        a_label+=(255 - a_label.get_dilate(3)).normalize(0,80);
        a_label.resize(-100,-100,1,3,1);
        return draw_image(0,is_down?height() - a_label.height():0,a_label,0.85f);
      }
      return *this;