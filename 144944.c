
    CImgDisplay& show() {
      if (is_empty() || !_is_closed) return *this;
      cimg_lock_display();
      if (_is_fullscreen) _init_fullscreen();
      _map_window();
      _is_closed = false;
      cimg_unlock_display();
      return paint();