
    const T& _atXYZC(const int x, const int y, const int z, const int c) const {
      return (*this)(cimg::cut(x,0,width() - 1),
                     cimg::cut(y,0,height() - 1),
                     cimg::cut(z,0,depth() - 1),
                     cimg::cut(c,0,spectrum() - 1));