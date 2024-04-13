    **/
    Tfloat cubic_atXYZ(const float fx, const float fy, const float fz, const int c, const T& out_value) const {
      const int
        x = (int)fx - (fx>=0?0:1), px = x - 1, nx = x + 1, ax = x + 2,
        y = (int)fy - (fy>=0?0:1), py = y - 1, ny = y + 1, ay = y + 2,
        z = (int)fz - (fz>=0?0:1), pz = z - 1, nz = z + 1, az = z + 2;
      const float dx = fx - x, dy = fy - y, dz = fz - z;
      const Tfloat
        Ippp = (Tfloat)atXYZ(px,py,pz,c,out_value), Icpp = (Tfloat)atXYZ(x,py,pz,c,out_value),
        Inpp = (Tfloat)atXYZ(nx,py,pz,c,out_value), Iapp = (Tfloat)atXYZ(ax,py,pz,c,out_value),
        Ipp = Icpp + 0.5f*(dx*(-Ippp + Inpp) + dx*dx*(2*Ippp - 5*Icpp + 4*Inpp - Iapp) +
                           dx*dx*dx*(-Ippp + 3*Icpp - 3*Inpp + Iapp)),
        Ipcp = (Tfloat)atXYZ(px,y,pz,c,out_value),  Iccp = (Tfloat)atXYZ(x, y,pz,c,out_value),
        Incp = (Tfloat)atXYZ(nx,y,pz,c,out_value),  Iacp = (Tfloat)atXYZ(ax,y,pz,c,out_value),
        Icp = Iccp + 0.5f*(dx*(-Ipcp + Incp) + dx*dx*(2*Ipcp - 5*Iccp + 4*Incp - Iacp) +
                           dx*dx*dx*(-Ipcp + 3*Iccp - 3*Incp + Iacp)),
        Ipnp = (Tfloat)atXYZ(px,ny,pz,c,out_value), Icnp = (Tfloat)atXYZ(x,ny,pz,c,out_value),
        Innp = (Tfloat)atXYZ(nx,ny,pz,c,out_value), Ianp = (Tfloat)atXYZ(ax,ny,pz,c,out_value),
        Inp = Icnp + 0.5f*(dx*(-Ipnp + Innp) + dx*dx*(2*Ipnp - 5*Icnp + 4*Innp - Ianp) +
                           dx*dx*dx*(-Ipnp + 3*Icnp - 3*Innp + Ianp)),
        Ipap = (Tfloat)atXYZ(px,ay,pz,c,out_value), Icap = (Tfloat)atXYZ(x,ay,pz,c,out_value),
        Inap = (Tfloat)atXYZ(nx,ay,pz,c,out_value), Iaap = (Tfloat)atXYZ(ax,ay,pz,c,out_value),
        Iap = Icap + 0.5f*(dx*(-Ipap + Inap) + dx*dx*(2*Ipap - 5*Icap + 4*Inap - Iaap) +
                           dx*dx*dx*(-Ipap + 3*Icap - 3*Inap + Iaap)),
        Ip = Icp + 0.5f*(dy*(-Ipp + Inp) + dy*dy*(2*Ipp - 5*Icp + 4*Inp - Iap) +
                         dy*dy*dy*(-Ipp + 3*Icp - 3*Inp + Iap)),
        Ippc = (Tfloat)atXYZ(px,py,z,c,out_value), Icpc = (Tfloat)atXYZ(x,py,z,c,out_value),
        Inpc = (Tfloat)atXYZ(nx,py,z,c,out_value), Iapc = (Tfloat)atXYZ(ax,py,z,c,out_value),
        Ipc = Icpc + 0.5f*(dx*(-Ippc + Inpc) + dx*dx*(2*Ippc - 5*Icpc + 4*Inpc - Iapc) +
                           dx*dx*dx*(-Ippc + 3*Icpc - 3*Inpc + Iapc)),
        Ipcc = (Tfloat)atXYZ(px,y,z,c,out_value),  Iccc = (Tfloat)atXYZ(x, y,z,c,out_value),
        Incc = (Tfloat)atXYZ(nx,y,z,c,out_value),  Iacc = (Tfloat)atXYZ(ax,y,z,c,out_value),
        Icc = Iccc + 0.5f*(dx*(-Ipcc + Incc) + dx*dx*(2*Ipcc - 5*Iccc + 4*Incc - Iacc) +
                           dx*dx*dx*(-Ipcc + 3*Iccc - 3*Incc + Iacc)),
        Ipnc = (Tfloat)atXYZ(px,ny,z,c,out_value), Icnc = (Tfloat)atXYZ(x,ny,z,c,out_value),
        Innc = (Tfloat)atXYZ(nx,ny,z,c,out_value), Ianc = (Tfloat)atXYZ(ax,ny,z,c,out_value),
        Inc = Icnc + 0.5f*(dx*(-Ipnc + Innc) + dx*dx*(2*Ipnc - 5*Icnc + 4*Innc - Ianc) +
                           dx*dx*dx*(-Ipnc + 3*Icnc - 3*Innc + Ianc)),
        Ipac = (Tfloat)atXYZ(px,ay,z,c,out_value), Icac = (Tfloat)atXYZ(x,ay,z,c,out_value),
        Inac = (Tfloat)atXYZ(nx,ay,z,c,out_value), Iaac = (Tfloat)atXYZ(ax,ay,z,c,out_value),
        Iac = Icac + 0.5f*(dx*(-Ipac + Inac) + dx*dx*(2*Ipac - 5*Icac + 4*Inac - Iaac) +
                           dx*dx*dx*(-Ipac + 3*Icac - 3*Inac + Iaac)),
        Ic = Icc + 0.5f*(dy*(-Ipc + Inc) + dy*dy*(2*Ipc - 5*Icc + 4*Inc - Iac) +
                         dy*dy*dy*(-Ipc + 3*Icc - 3*Inc + Iac)),
        Ippn = (Tfloat)atXYZ(px,py,nz,c,out_value), Icpn = (Tfloat)atXYZ(x,py,nz,c,out_value),
        Inpn = (Tfloat)atXYZ(nx,py,nz,c,out_value), Iapn = (Tfloat)atXYZ(ax,py,nz,c,out_value),
        Ipn = Icpn + 0.5f*(dx*(-Ippn + Inpn) + dx*dx*(2*Ippn - 5*Icpn + 4*Inpn - Iapn) +
                           dx*dx*dx*(-Ippn + 3*Icpn - 3*Inpn + Iapn)),
        Ipcn = (Tfloat)atXYZ(px,y,nz,c,out_value),  Iccn = (Tfloat)atXYZ(x, y,nz,c,out_value),
        Incn = (Tfloat)atXYZ(nx,y,nz,c,out_value),  Iacn = (Tfloat)atXYZ(ax,y,nz,c,out_value),
        Icn = Iccn + 0.5f*(dx*(-Ipcn + Incn) + dx*dx*(2*Ipcn - 5*Iccn + 4*Incn - Iacn) +
                           dx*dx*dx*(-Ipcn + 3*Iccn - 3*Incn + Iacn)),
        Ipnn = (Tfloat)atXYZ(px,ny,nz,c,out_value), Icnn = (Tfloat)atXYZ(x,ny,nz,c,out_value),
        Innn = (Tfloat)atXYZ(nx,ny,nz,c,out_value), Iann = (Tfloat)atXYZ(ax,ny,nz,c,out_value),
        Inn = Icnn + 0.5f*(dx*(-Ipnn + Innn) + dx*dx*(2*Ipnn - 5*Icnn + 4*Innn - Iann) +
                           dx*dx*dx*(-Ipnn + 3*Icnn - 3*Innn + Iann)),
        Ipan = (Tfloat)atXYZ(px,ay,nz,c,out_value), Ican = (Tfloat)atXYZ(x,ay,nz,c,out_value),
        Inan = (Tfloat)atXYZ(nx,ay,nz,c,out_value), Iaan = (Tfloat)atXYZ(ax,ay,nz,c,out_value),
        Ian = Ican + 0.5f*(dx*(-Ipan + Inan) + dx*dx*(2*Ipan - 5*Ican + 4*Inan - Iaan) +
                           dx*dx*dx*(-Ipan + 3*Ican - 3*Inan + Iaan)),
        In = Icn + 0.5f*(dy*(-Ipn + Inn) + dy*dy*(2*Ipn - 5*Icn + 4*Inn - Ian) +
                         dy*dy*dy*(-Ipn + 3*Icn - 3*Inn + Ian)),
        Ippa = (Tfloat)atXYZ(px,py,az,c,out_value), Icpa = (Tfloat)atXYZ(x,py,az,c,out_value),
        Inpa = (Tfloat)atXYZ(nx,py,az,c,out_value), Iapa = (Tfloat)atXYZ(ax,py,az,c,out_value),
        Ipa = Icpa + 0.5f*(dx*(-Ippa + Inpa) + dx*dx*(2*Ippa - 5*Icpa + 4*Inpa - Iapa) +
                           dx*dx*dx*(-Ippa + 3*Icpa - 3*Inpa + Iapa)),
        Ipca = (Tfloat)atXYZ(px,y,az,c,out_value),  Icca = (Tfloat)atXYZ(x, y,az,c,out_value),
        Inca = (Tfloat)atXYZ(nx,y,az,c,out_value),  Iaca = (Tfloat)atXYZ(ax,y,az,c,out_value),
        Ica = Icca + 0.5f*(dx*(-Ipca + Inca) + dx*dx*(2*Ipca - 5*Icca + 4*Inca - Iaca) +
                           dx*dx*dx*(-Ipca + 3*Icca - 3*Inca + Iaca)),
        Ipna = (Tfloat)atXYZ(px,ny,az,c,out_value), Icna = (Tfloat)atXYZ(x,ny,az,c,out_value),
        Inna = (Tfloat)atXYZ(nx,ny,az,c,out_value), Iana = (Tfloat)atXYZ(ax,ny,az,c,out_value),
        Ina = Icna + 0.5f*(dx*(-Ipna + Inna) + dx*dx*(2*Ipna - 5*Icna + 4*Inna - Iana) +
                           dx*dx*dx*(-Ipna + 3*Icna - 3*Inna + Iana)),
        Ipaa = (Tfloat)atXYZ(px,ay,az,c,out_value), Icaa = (Tfloat)atXYZ(x,ay,az,c,out_value),
        Inaa = (Tfloat)atXYZ(nx,ay,az,c,out_value), Iaaa = (Tfloat)atXYZ(ax,ay,az,c,out_value),
        Iaa = Icaa + 0.5f*(dx*(-Ipaa + Inaa) + dx*dx*(2*Ipaa - 5*Icaa + 4*Inaa - Iaaa) +
                           dx*dx*dx*(-Ipaa + 3*Icaa - 3*Inaa + Iaaa)),
        Ia = Ica + 0.5f*(dy*(-Ipa + Ina) + dy*dy*(2*Ipa - 5*Ica + 4*Ina - Iaa) +
                         dy*dy*dy*(-Ipa + 3*Ica - 3*Ina + Iaa));
      return Ic + 0.5f*(dz*(-Ip + In) + dz*dz*(2*Ip - 5*Ic + 4*In - Ia) + dz*dz*dz*(-Ip + 3*Ic - 3*In + Ia));