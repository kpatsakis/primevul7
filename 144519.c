    **/
    CImgDisplay& resize(const bool force_redraw=true) {
      resize(window_width(),window_height(),force_redraw);
      return *this;