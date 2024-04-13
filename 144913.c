    **/
    CImg<T>& assign() {
      if (!_is_shared) delete[] _data;
      _width = _height = _depth = _spectrum = 0; _is_shared = false; _data = 0;
      return *this;