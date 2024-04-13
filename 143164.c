
    CImgDisplay& show_mouse() {
      if (is_empty()) return *this;
      _is_cursor_visible = true;
      return *this;