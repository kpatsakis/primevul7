
    T& operator()(const unsigned int x, const unsigned int y) {
      return _data[x + y*_width];