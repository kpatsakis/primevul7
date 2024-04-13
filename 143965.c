    **/
    Tfloat linear_atX(const float fx, const int y, const int z, const int c, const T& out_value) const {
      const int
	x = (int)fx - (fx>=0?0:1), nx = x + 1;
      const float
        dx = fx - x;
      const Tfloat
        Ic = (Tfloat)atX(x,y,z,c,out_value), In = (Tfloat)atXY(nx,y,z,c,out_value);
      return Ic + dx*(In - Ic);