    **/
    Tfloat linear_atXY(const float fx, const float fy, const int z, const int c, const T& out_value) const {
      const int
        x = (int)fx - (fx>=0?0:1), nx = x + 1,
        y = (int)fy - (fy>=0?0:1), ny = y + 1;
      const float
        dx = fx - x,
        dy = fy - y;
      const Tfloat
        Icc = (Tfloat)atXY(x,y,z,c,out_value),  Inc = (Tfloat)atXY(nx,y,z,c,out_value),
        Icn = (Tfloat)atXY(x,ny,z,c,out_value), Inn = (Tfloat)atXY(nx,ny,z,c,out_value);
      return Icc + dx*(Inc - Icc + dy*(Icc + Inn - Icn - Inc)) + dy*(Icn - Icc);