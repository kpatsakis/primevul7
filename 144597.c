
    CImgDisplay& close() {
      if (is_empty() || _is_closed) return *this;
      Display *const dpy = cimg::X11_attr().display;
      cimg_lock_display();
      if (_is_fullscreen) _desinit_fullscreen();
      XUnmapWindow(dpy,_window);
      _window_x = _window_y = -1;
      _is_closed = true;
      cimg_unlock_display();
      return *this;