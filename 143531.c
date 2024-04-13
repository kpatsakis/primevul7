
    T _atNXYZC(const int pos, const int x, const int y, const int z, const int c) const {
      return _data[cimg::cut(pos,0,width() - 1)].atXYZC(x,y,z,c);