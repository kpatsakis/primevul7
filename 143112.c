
    const T& _at(const int offset) const {
      const unsigned int siz = (unsigned int)size();
      return (*this)[offset<0?0:(unsigned int)offset>=siz?siz - 1:offset];