
    Tfloat _cubic_atX_p(const float fx, const int y=0, const int z=0, const int c=0) const {
      const float
        nfx = cimg::type<float>::is_nan(fx)?0:cimg::mod(fx,(float)_width);
      const int
        x = (int)nfx;
      const float
        dx = nfx - x;
      const int
        px = cimg::mod(x - 1,width()), nx = cimg::mod(x + 1,width()), ax = cimg::mod(x + 2,width());
      const Tfloat
        Ip = (Tfloat)(*this)(px,y,z,c), Ic = (Tfloat)(*this)(x,y,z,c),
        In = (Tfloat)(*this)(nx,y,z,c), Ia = (Tfloat)(*this)(ax,y,z,c);
      return Ic + 0.5f*(dx*(-Ip + In) + dx*dx*(2*Ip - 5*Ic + 4*In - Ia) + dx*dx*dx*(-Ip + 3*Ic - 3*In + Ia));