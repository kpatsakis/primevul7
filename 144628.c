
    T _atNXY(const int pos, const int x, const int y, const int z=0, const int c=0) const {
      return _data[cimg::cut(pos,0,width() - 1)].atXY(x,y,z,c);