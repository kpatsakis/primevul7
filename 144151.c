     **/
    CImg<T>& matrix() {
      const ulongT siz = size();
      switch (siz) {
      case 1 : break;
      case 4 : _width = _height = 2; break;
      case 9 : _width = _height = 3; break;
      case 16 : _width = _height = 4; break;
      case 25 : _width = _height = 5; break;
      case 36 : _width = _height = 6; break;
      case 49 : _width = _height = 7; break;
      case 64 : _width = _height = 8; break;
      case 81 : _width = _height = 9; break;
      case 100 : _width = _height = 10; break;
      default : {
        ulongT i = 11, i2 = i*i;
        while (i2<siz) { i2+=2*i + 1; ++i; }
        if (i2==siz) _width = _height = i;
        else throw CImgInstanceException(_cimg_instance
                                         "matrix(): Invalid instance size %u (should be a square integer).",
                                         cimg_instance,
                                         siz);
      }
      }
      return *this;