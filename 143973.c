
    const T& operator()(const unsigned int x, const unsigned int y, const unsigned int z, const unsigned int c) const {
      return _data[x + y*(ulongT)_width + z*(ulongT)_width*_height + c*(ulongT)_width*_height*_depth];