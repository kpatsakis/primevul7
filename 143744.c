
    T& _atXY(const int x, const int y, const int z=0, const int c=0) {
      return (*this)(cimg::cut(x,0,width() - 1),
                     cimg::cut(y,0,height() - 1),z,c);