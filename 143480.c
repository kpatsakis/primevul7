
    Tfloat _linear_atXYZ_p(const float fx, const float fy=0, const float fz=0, const int c=0) const {
      const float
        nfx = cimg::mod(fx,(float)_width),
        nfy = cimg::mod(fy,(float)_height),
        nfz = cimg::mod(fz,(float)_depth);
      const unsigned int
        x = (unsigned int)nfx,
        y = (unsigned int)nfy,
        z = (unsigned int)nfz;
      const float
        dx = nfx - x,
        dy = nfy - y,
        dz = nfz - z;
      const unsigned int
        nx = cimg::mod(x + 1,_width),
        ny = cimg::mod(y + 1,_height),
        nz = cimg::mod(z + 1,_depth);
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