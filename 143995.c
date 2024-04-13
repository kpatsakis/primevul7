
    CImgDisplay& assign(const CImgDisplay& disp) {
      if (!disp) return assign();
      _assign(disp._width,disp._height,disp._title,disp._normalization,disp._is_fullscreen,disp._is_closed);
      std::memcpy(_data,disp._data,sizeof(unsigned int)*_width*_height);
      return paint();