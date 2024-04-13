
    CImgDisplay& set_title(const char *const format, ...) {
      if (is_empty()) return *this;
      char *const tmp = new char[1024];
      va_list ap;
      va_start(ap, format);
      cimg_vsnprintf(tmp,1024,format,ap);
      va_end(ap);
      if (!std::strcmp(_title,tmp)) { delete[] tmp; return *this; }
      delete[] _title;
      const unsigned int s = (unsigned int)std::strlen(tmp) + 1;
      _title = new char[s];
      std::memcpy(_title,tmp,s*sizeof(char));
      Display *const dpy = cimg::X11_attr().display;
      cimg_lock_display();
      XStoreName(dpy,_window,tmp);
      cimg_unlock_display();
      delete[] tmp;
      return *this;