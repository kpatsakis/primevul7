
    Tfloat _linear_atXY_p(const float fx, const float fy, const int z=0, const int c=0) const {
      const float
        nfx = cimg::mod(fx,(float)_width),
        nfy = cimg::mod(fy,(float)_height);
      const unsigned int
        x = (unsigned int)nfx,
        y = (unsigned int)nfy;
      const float
        dx = nfx - x,
        dy = nfy - y;
      const unsigned int
        nx = cimg::mod(x + 1,_width),
        ny = cimg::mod(y + 1,_height);
      const Tfloat
        Icc = (Tfloat)(*this)(x,y,z,c),  Inc = (Tfloat)(*this)(nx,y,z,c),
        Icn = (Tfloat)(*this)(x,ny,z,c), Inn = (Tfloat)(*this)(nx,ny,z,c);
      return Icc + dx*(Inc - Icc + dy*(Icc + Inn - Icn - Inc)) + dy*(Icn - Icc);