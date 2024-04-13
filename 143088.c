
    CImgDisplay& assign() {
      if (is_empty()) return flush();
      DestroyWindow(_window);
      TerminateThread(_thread,0);
      delete[] _data;
      delete[] _title;
      _data = 0;
      _title = 0;
      if (_is_fullscreen) _desinit_fullscreen();
      _width = _height = _normalization = _window_width = _window_height = 0;
      _window_x = _window_y = 0;
      _is_fullscreen = false;
      _is_closed = true;
      _min = _max = 0;
      _title = 0;
      flush();
      return *this;