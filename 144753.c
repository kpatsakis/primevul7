    **/
    CImgDisplay& set_fullscreen(const bool is_fullscreen, const bool force_redraw=true) {
      if (is_empty() || _is_fullscreen==is_fullscreen) return *this;
      return toggle_fullscreen(force_redraw);