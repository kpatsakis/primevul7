
    Tfloat _linear_atX_p(const float fx, const int y=0, const int z=0, const int c=0) const {
      const float
        nfx = cimg::mod(fx,(float)_width);
      const unsigned int
        x = (unsigned int)nfx;
      const float
        dx = nfx - x;
      const unsigned int
        nx = cimg::mod(x + 1,_width);
      const Tfloat
        Ic = (Tfloat)(*this)(x,y,z,c), In = (Tfloat)(*this)(nx,y,z,c);
      return Ic + dx*(In - Ic);