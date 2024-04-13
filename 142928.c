    **/
    CImg<T>& transpose() {
      if (_width==1) { _width = _height; _height = 1; return *this; }
      if (_height==1) { _height = _width; _width = 1; return *this; }
      if (_width==_height) {
        cimg_forYZC(*this,y,z,c) for (int x = y; x<width(); ++x) cimg::swap((*this)(x,y,z,c),(*this)(y,x,z,c));
        return *this;
      }
      return get_transpose().move_to(*this);