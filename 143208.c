
    T _atNX(const int pos, const int x, const int y=0, const int z=0, const int c=0) const {
      return _data[cimg::cut(pos,0,width() - 1)].atX(x,y,z,c);