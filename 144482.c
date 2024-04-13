    **/
    CImgDisplay& resize(const CImgDisplay& disp, const bool force_redraw=true) {
      return resize(disp.width(),disp.height(),force_redraw);