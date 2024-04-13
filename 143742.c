    **/
    CImg<T>& unroll(const char axis) {
      const unsigned int siz = (unsigned int)size();
      if (siz) switch (cimg::lowercase(axis)) {
      case 'x' : _width = siz; _height = _depth = _spectrum = 1; break;
      case 'y' : _height = siz; _width = _depth = _spectrum = 1; break;
      case 'z' : _depth = siz; _width = _height = _spectrum = 1; break;
      default : _spectrum = siz; _width = _height = _depth = 1;
      }
      return *this;