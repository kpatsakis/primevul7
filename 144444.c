
    const T& _atX(const int x, const int y=0, const int z=0, const int c=0) const {
      return (*this)(x<0?0:(x>=width()?width() - 1:x),y,z,c);