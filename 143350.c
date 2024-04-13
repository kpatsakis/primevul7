
    CImgDisplay& assign(const CImgDisplay& disp) {
      if (!disp) return assign();
      _assign(disp._width,disp._height,disp._title,disp._normalization,disp._is_fullscreen,disp._is_closed);
      std::memcpy(_data,disp._data,(cimg::X11_attr().nb_bits==8?sizeof(unsigned char):
                                    cimg::X11_attr().nb_bits==16?sizeof(unsigned short):
                                    sizeof(unsigned int))*(size_t)_width*_height);
      return paint();