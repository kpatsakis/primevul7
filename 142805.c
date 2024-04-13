
    T& operator()(const unsigned int x, const unsigned int y, const unsigned int z, const unsigned int,
                  const ulongT wh) {
      return _data[x + y*_width + z*wh];