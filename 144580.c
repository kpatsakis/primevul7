    // Move current display window so that its content stays inside the current screen.
    CImgDisplay& move_inside_screen() {
      if (is_empty()) return *this;
      const int
        x0 = window_x(),
        y0 = window_y(),
        x1 = x0 + window_width() - 1,
        y1 = y0 + window_height() - 1,
        sw = CImgDisplay::screen_width(),
        sh = CImgDisplay::screen_height();
      if (x0<0 || y0<0 || x1>=sw || y1>=sh)
        move(std::max(0,std::min(x0,sw - x1 + x0)),
             std::max(0,std::min(y0,sh - y1 + y0)));
      return *this;