
    Tfloat _linear_atX(const float fx, const int y=0, const int z=0, const int c=0) const {
      const float
        nfx = cimg::cut(fx,0,width() - 1);
      const unsigned int
        x = (unsigned int)nfx;
      const float
        dx = nfx - x;
      const unsigned int
        nx = dx>0?x + 1:x;
      const Tfloat
        Ic = (Tfloat)(*this)(x,y,z,c), In = (Tfloat)(*this)(nx,y,z,c);
      return Ic + dx*(In - Ic);