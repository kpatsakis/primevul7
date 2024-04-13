
    CImgDisplay& set_mouse(const int posx, const int posy) {
      if (is_empty() || _is_closed || posx<0 || posy<0) return *this;
      _update_window_pos();
      const int res = (int)SetCursorPos(_window_x + posx,_window_y + posy);
      if (res) { _mouse_x = posx; _mouse_y = posy; }
      return *this;