
    CImgDisplay& set_mouse(const int posx, const int posy) {
      if (is_empty() || _is_closed) return *this;
      Display *const dpy = cimg::X11_attr().display;
      cimg_lock_display();
      XWarpPointer(dpy,0L,_window,0,0,0,0,posx,posy);
      _mouse_x = posx; _mouse_y = posy;
      _is_moved = false;
      XSync(dpy,0);
      cimg_unlock_display();
      return *this;