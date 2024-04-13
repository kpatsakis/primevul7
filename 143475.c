
    Tfloat _linear_atXY(const float fx, const float fy, const int z=0, const int c=0) const {
      const float
        nfx = cimg::cut(fx,0,width() - 1),
        nfy = cimg::cut(fy,0,height() - 1);
      const unsigned int
        x = (unsigned int)nfx,
        y = (unsigned int)nfy;
      const float
        dx = nfx - x,
        dy = nfy - y;
      const unsigned int
        nx = dx>0?x + 1:x,
        ny = dy>0?y + 1:y;
      const Tfloat
        Icc = (Tfloat)(*this)(x,y,z,c),  Inc = (Tfloat)(*this)(nx,y,z,c),
        Icn = (Tfloat)(*this)(x,ny,z,c), Inn = (Tfloat)(*this)(nx,ny,z,c);
      return Icc + dx*(Inc - Icc + dy*(Icc + Inn - Icn - Inc)) + dy*(Icn - Icc);