    **/
    T& atXYZ(const int x, const int y, const int z, const int c, const T& out_value) {
      return (x<0 || y<0 || z<0 || x>=width() || y>=height() || z>=depth())?
        (cimg::temporary(out_value)=out_value):(*this)(x,y,z,c);