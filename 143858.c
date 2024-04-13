
    T _atN(const int pos, const int x=0, const int y=0, const int z=0, const int c=0) const {
      return _data[cimg::cut(pos,0,width() - 1)](x,y,z,c);