    **/
    CImgDisplay& assign(const CImgDisplay &disp) {
      _no_display_exception();
      return assign(disp._width,disp._height);