    **/
    CImgDisplay& resize(const int width, const int height, const bool force_redraw=true) {
      return assign(width,height,0,3,force_redraw);