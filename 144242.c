
    Tfloat _linear_atXYZ(const float fx, const float fy=0, const float fz=0, const int c=0) const {
      const float
        nfx = cimg::cut(fx,0,width() - 1),
        nfy = cimg::cut(fy,0,height() - 1),
        nfz = cimg::cut(fz,0,depth() - 1);
      const unsigned int
        x = (unsigned int)nfx,
        y = (unsigned int)nfy,
        z = (unsigned int)nfz;
      const float
        dx = nfx - x,
        dy = nfy - y,
        dz = nfz - z;
      const unsigned int
        nx = dx>0?x + 1:x,
        ny = dy>0?y + 1:y,
        nz = dz>0?z + 1:z;
      const Tfloat
        Iccc = (Tfloat)(*this)(x,y,z,c), Incc = (Tfloat)(*this)(nx,y,z,c),
        Icnc = (Tfloat)(*this)(x,ny,z,c), Innc = (Tfloat)(*this)(nx,ny,z,c),
        Iccn = (Tfloat)(*this)(x,y,nz,c), Incn = (Tfloat)(*this)(nx,y,nz,c),
        Icnn = (Tfloat)(*this)(x,ny,nz,c), Innn = (Tfloat)(*this)(nx,ny,nz,c);
      return Iccc +
        dx*(Incc - Iccc +
            dy*(Iccc + Innc - Icnc - Incc +
                dz*(Iccn + Innn + Icnc + Incc - Icnn - Incn - Iccc - Innc)) +
            dz*(Iccc + Incn - Iccn - Incc)) +
        dy*(Icnc - Iccc +
            dz*(Iccc + Icnn - Iccn - Icnc)) +
        dz*(Iccn - Iccc);