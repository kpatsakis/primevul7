
    CImgDisplay& _update_window_pos() {
      if (_is_closed) _window_x = _window_y = -1;
      else {
        RECT rect;
        rect.left = rect.top = 0; rect.right = (LONG)_width - 1; rect.bottom = (LONG)_height - 1;
        AdjustWindowRect(&rect,WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,false);
        const int
          border1 = (int)((rect.right - rect.left + 1 - _width)/2),
          border2 = (int)(rect.bottom - rect.top + 1 - _height - border1);
        GetWindowRect(_window,&rect);
        _window_x = rect.left + border1;
        _window_y = rect.top + border2;
      }
      return *this;