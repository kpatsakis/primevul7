    **/
    Tfloat linear_atXYZ(const float fx, const float fy, const float fz, const int c, const T& out_value) const {
      const int
        x = (int)fx - (fx>=0?0:1), nx = x + 1,
        y = (int)fy - (fy>=0?0:1), ny = y + 1,
        z = (int)fz - (fz>=0?0:1), nz = z + 1;
      const float
        dx = fx - x,
        dy = fy - y,
        dz = fz - z;
      const Tfloat
        Iccc = (Tfloat)atXYZ(x,y,z,c,out_value), Incc = (Tfloat)atXYZ(nx,y,z,c,out_value),
        Icnc = (Tfloat)atXYZ(x,ny,z,c,out_value), Innc = (Tfloat)atXYZ(nx,ny,z,c,out_value),
        Iccn = (Tfloat)atXYZ(x,y,nz,c,out_value), Incn = (Tfloat)atXYZ(nx,y,nz,c,out_value),
        Icnn = (Tfloat)atXYZ(x,ny,nz,c,out_value), Innn = (Tfloat)atXYZ(nx,ny,nz,c,out_value);
      return Iccc +
        dx*(Incc - Iccc +
            dy*(Iccc + Innc - Icnc - Incc +
                dz*(Iccn + Innn + Icnc + Incc - Icnn - Incn - Iccc - Innc)) +
            dz*(Iccc + Incn - Iccn - Incc)) +
        dy*(Icnc - Iccc +
            dz*(Iccc + Icnn - Iccn - Icnc)) +
        dz*(Iccn - Iccc);