
    CImgDisplay& move(const int posx, const int posy) {
      if (is_empty()) return *this;
      if (_window_x!=posx || _window_y!=posy) {
        show();
        Display *const dpy = cimg::X11_attr().display;
        cimg_lock_display();
        XMoveWindow(dpy,_window,posx,posy);
        _window_x = posx; _window_y = posy;
        cimg_unlock_display();
      }
      _is_moved = false;
      return paint();