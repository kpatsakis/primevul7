    **/
    T& atX(const int x, const int y, const int z, const int c, const T& out_value) {
      return (x<0 || x>=width())?(cimg::temporary(out_value)=out_value):(*this)(x,y,z,c);