    **/
    Tfloat cubic_atX(const float fx, const int y, const int z, const int c, const T& out_value) const {
      const int
        x = (int)fx - (fx>=0?0:1), px = x - 1, nx = x + 1, ax = x + 2;
      const float
        dx = fx - x;
      const Tfloat
        Ip = (Tfloat)atX(px,y,z,c,out_value), Ic = (Tfloat)atX(x,y,z,c,out_value),
        In = (Tfloat)atX(nx,y,z,c,out_value), Ia = (Tfloat)atX(ax,y,z,c,out_value);
      return Ic + 0.5f*(dx*(-Ip + In) + dx*dx*(2*Ip - 5*Ic + 4*In - Ia) + dx*dx*dx*(-Ip + 3*Ic - 3*In + Ia));