    **/
    Tfloat linear_atXYZC(const float fx, const float fy, const float fz, const float fc, const T& out_value) const {
      const int
        x = (int)fx - (fx>=0?0:1), nx = x + 1,
        y = (int)fy - (fy>=0?0:1), ny = y + 1,
        z = (int)fz - (fz>=0?0:1), nz = z + 1,
        c = (int)fc - (fc>=0?0:1), nc = c + 1;
      const float
        dx = fx - x,
        dy = fy - y,
        dz = fz - z,
        dc = fc - c;
      const Tfloat
        Icccc = (Tfloat)atXYZC(x,y,z,c,out_value), Inccc = (Tfloat)atXYZC(nx,y,z,c,out_value),
        Icncc = (Tfloat)atXYZC(x,ny,z,c,out_value), Inncc = (Tfloat)atXYZC(nx,ny,z,c,out_value),
        Iccnc = (Tfloat)atXYZC(x,y,nz,c,out_value), Incnc = (Tfloat)atXYZC(nx,y,nz,c,out_value),
        Icnnc = (Tfloat)atXYZC(x,ny,nz,c,out_value), Innnc = (Tfloat)atXYZC(nx,ny,nz,c,out_value),
        Icccn = (Tfloat)atXYZC(x,y,z,nc,out_value), Inccn = (Tfloat)atXYZC(nx,y,z,nc,out_value),
        Icncn = (Tfloat)atXYZC(x,ny,z,nc,out_value), Inncn = (Tfloat)atXYZC(nx,ny,z,nc,out_value),
        Iccnn = (Tfloat)atXYZC(x,y,nz,nc,out_value), Incnn = (Tfloat)atXYZC(nx,y,nz,nc,out_value),
        Icnnn = (Tfloat)atXYZC(x,ny,nz,nc,out_value), Innnn = (Tfloat)atXYZC(nx,ny,nz,nc,out_value);
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
        dc*(Icccn  -Icccc);