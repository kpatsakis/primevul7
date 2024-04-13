    **/
    CImgDisplay& toggle_fullscreen(const bool force_redraw=true) {
      return assign(_width,_height,0,3,force_redraw);