
    CImgDisplay& close() {
      if (is_empty() || _is_closed) return *this;
      _is_closed = true;
      if (_is_fullscreen) _desinit_fullscreen();
      ShowWindow(_window,SW_HIDE);
      _window_x = _window_y = 0;
      return *this;