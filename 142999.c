
    T& operator()(const unsigned int x, const unsigned int y, const unsigned int z, const unsigned int c,
                  const ulongT wh, const ulongT whd) {
      return _data[x + y*_width + z*wh + c*whd];