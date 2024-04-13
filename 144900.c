
    void _desinit_fullscreen() {
      if (!_is_fullscreen) return;
      if (_background_window) DestroyWindow(_background_window);
      _background_window = 0;
      if (_curr_mode.dmSize) ChangeDisplaySettings(&_curr_mode,0);
      _is_fullscreen = false;