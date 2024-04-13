
    CImgDisplay& show() {
      if (is_empty() || !_is_closed) return *this;
      _is_closed = false;
      if (_is_fullscreen) _init_fullscreen();
      ShowWindow(_window,SW_SHOW);
      _update_window_pos();
      return paint();