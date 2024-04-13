
    CImgDisplay& show_mouse() {
      if (is_empty()) return *this;
      Display *const dpy = cimg::X11_attr().display;
      cimg_lock_display();
      XUndefineCursor(dpy,_window);
      cimg_unlock_display();
      return *this;