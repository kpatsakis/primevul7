
    Tfloat _linear_atXYZC_p(const float fx, const float fy=0, const float fz=0, const float fc=0) const {
      const float
        nfx = cimg::mod(fx,(float)_width),
        nfy = cimg::mod(fy,(float)_height),
        nfz = cimg::mod(fz,(float)_depth),
        nfc = cimg::mod(fc,(float)_spectrum);
      const unsigned int
        x = (unsigned int)nfx,
        y = (unsigned int)nfy,
        z = (unsigned int)nfz,
        c = (unsigned int)nfc;
      const float
        dx = nfx - x,
        dy = nfy - y,
        dz = nfz - z,
        dc = nfc - c;
      const unsigned int
        nx = cimg::mod(x + 1,_width),
        ny = cimg::mod(y + 1,_height),
        nz = cimg::mod(z + 1,_depth),
        nc = cimg::mod(c + 1,_spectrum);
      const Tfloat
        Icccc = (Tfloat)(*this)(x,y,z,c), Inccc = (Tfloat)(*this)(nx,y,z,c),
        Icncc = (Tfloat)(*this)(x,ny,z,c), Inncc = (Tfloat)(*this)(nx,ny,z,c),
        Iccnc = (Tfloat)(*this)(x,y,nz,c), Incnc = (Tfloat)(*this)(nx,y,nz,c),
        Icnnc = (Tfloat)(*this)(x,ny,nz,c), Innnc = (Tfloat)(*this)(nx,ny,nz,c),
        Icccn = (Tfloat)(*this)(x,y,z,nc), Inccn = (Tfloat)(*this)(nx,y,z,nc),
        Icncn = (Tfloat)(*this)(x,ny,z,nc), Inncn = (Tfloat)(*this)(nx,ny,z,nc),
        Iccnn = (Tfloat)(*this)(x,y,nz,nc), Incnn = (Tfloat)(*this)(nx,y,nz,nc),
        Icnnn = (Tfloat)(*this)(x,ny,nz,nc), Innnn = (Tfloat)(*this)(nx,ny,nz,nc);
      return Icccc +
        dx*(Inccc - Icccc +
            dy*(Icccc + Inncc - Icncc - Inccc +
                dz*(Iccnc + Innnc + Icncc + Inccc - Icnnc - Incnc - Icccc - Inncc +
                    dc*(Iccnn + Innnn + Icncn + Inccn + Icnnc + Incnc + Icccc + Inncc -
                        Icnnn - Incnn - Icccn - Inncn - Iccnc - Innnc - Icncc - Inccc)) +
                dc*(Icccn + Inncn + Icncc + Inccc - Icncn - Inccn - Icccc - Inncc)) +
            dz*(Icccc + Incnc - Iccnc - Inccc +
                dc*(Icccn + Incnn + Iccnc + Inccc - Iccnn - Inccn - Icccc - Incnc)) +
            dc*(Icccc + Inccn - Inccc - Icccn)) +
        dy*(Icncc - Icccc +
            dz*(Icccc + Icnnc - Iccnc - Icncc +
                dc*(Icccn + Icnnn + Iccnc + Icncc - Iccnn - Icncn - Icccc - Icnnc)) +
            dc*(Icccc + Icncn - Icncc - Icccn)) +
        dz*(Iccnc - Icccc +
            dc*(Icccc + Iccnn - Iccnc - Icccn)) +
        dc*(Icccn - Icccc);