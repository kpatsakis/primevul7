    **/
    Tfloat cubic_atXY(const float fx, const float fy, const int z, const int c, const T& out_value) const {
      const int
        x = (int)fx - (fx>=0?0:1), px = x - 1, nx = x + 1, ax = x + 2,
        y = (int)fy - (fy>=0?0:1), py = y - 1, ny = y + 1, ay = y + 2;
      const float dx = fx - x, dy = fy - y;
      const Tfloat
        Ipp = (Tfloat)atXY(px,py,z,c,out_value), Icp = (Tfloat)atXY(x,py,z,c,out_value),
        Inp = (Tfloat)atXY(nx,py,z,c,out_value), Iap = (Tfloat)atXY(ax,py,z,c,out_value),
        Ip = Icp + 0.5f*(dx*(-Ipp + Inp) + dx*dx*(2*Ipp - 5*Icp + 4*Inp - Iap) + dx*dx*dx*(-Ipp + 3*Icp - 3*Inp + Iap)),
        Ipc = (Tfloat)atXY(px,y,z,c,out_value),  Icc = (Tfloat)atXY(x, y,z,c,out_value),
        Inc = (Tfloat)atXY(nx,y,z,c,out_value),  Iac = (Tfloat)atXY(ax,y,z,c,out_value),
        Ic = Icc + 0.5f*(dx*(-Ipc + Inc) + dx*dx*(2*Ipc - 5*Icc + 4*Inc - Iac) + dx*dx*dx*(-Ipc + 3*Icc - 3*Inc + Iac)),
        Ipn = (Tfloat)atXY(px,ny,z,c,out_value), Icn = (Tfloat)atXY(x,ny,z,c,out_value),
        Inn = (Tfloat)atXY(nx,ny,z,c,out_value), Ian = (Tfloat)atXY(ax,ny,z,c,out_value),
        In = Icn + 0.5f*(dx*(-Ipn + Inn) + dx*dx*(2*Ipn - 5*Icn + 4*Inn - Ian) + dx*dx*dx*(-Ipn + 3*Icn - 3*Inn + Ian)),
        Ipa = (Tfloat)atXY(px,ay,z,c,out_value), Ica = (Tfloat)atXY(x,ay,z,c,out_value),
        Ina = (Tfloat)atXY(nx,ay,z,c,out_value), Iaa = (Tfloat)atXY(ax,ay,z,c,out_value),
        Ia = Ica + 0.5f*(dx*(-Ipa + Ina) + dx*dx*(2*Ipa - 5*Ica + 4*Ina - Iaa) + dx*dx*dx*(-Ipa + 3*Ica - 3*Ina + Iaa));
      return Ic + 0.5f*(dy*(-Ip + In) + dy*dy*(2*Ip - 5*Ic + 4*In - Ia) + dy*dy*dy*(-Ip + 3*Ic - 3*In + Ia));