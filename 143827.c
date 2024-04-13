
    CImgDisplay& hide_mouse() {
      if (is_empty()) return *this;
      _is_cursor_visible = false;
      return *this;