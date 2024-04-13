
    const T& _atXYZ(const int x, const int y, const int z, const int c=0) const {
      return (*this)(cimg::cut(x,0,width() - 1),
                     cimg::cut(y,0,height() - 1),
                     cimg::cut(z,0,depth() - 1),c);