
    const T& operator()(const unsigned int x, const unsigned int y) const {
      return _data[x + y*_width];