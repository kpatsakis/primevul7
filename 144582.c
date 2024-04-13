
    T _atNXYZ(const int pos, const int x, const int y, const int z, const int c=0) const {
      return _data[cimg::cut(pos,0,width() - 1)].atXYZ(x,y,z,c);