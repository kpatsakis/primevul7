
    CImgDisplay& move(const int posx, const int posy) {
      if (is_empty()) return *this;
      if (_window_x!=posx || _window_y!=posy) {
        if (!_is_fullscreen) {
          RECT rect;
          rect.left = rect.top = 0; rect.right = (LONG)_window_width - 1; rect.bottom = (LONG)_window_height - 1;
          AdjustWindowRect(&rect,WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,false);
          const int
            border1 = (int)((rect.right - rect.left + 1 -_width)/2),
            border2 = (int)(rect.bottom - rect.top + 1 - _height - border1);
          SetWindowPos(_window,0,posx - border1,posy - border2,0,0,SWP_NOSIZE | SWP_NOZORDER);
        } else SetWindowPos(_window,0,posx,posy,0,0,SWP_NOSIZE | SWP_NOZORDER);
        _window_x = posx;
        _window_y = posy;
        show();
      }
      _is_moved = false;
      return *this;