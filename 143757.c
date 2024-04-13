    template<typename t>
    CImg<ulongT> get_label(const CImg<t>& connectivity_mask,
                           const Tfloat tolerance=0) const {
      int nb = 0;
      cimg_for(connectivity_mask,ptr,t) if (*ptr) ++nb;
      CImg<intT> dx(nb,1,1,1,0), dy(nb,1,1,1,0), dz(nb,1,1,1,0);
      nb = 0;
      cimg_forXYZ(connectivity_mask,x,y,z) if ((x || y || z) &&
                                               connectivity_mask(x,y,z)) {
        dx[nb] = x; dy[nb] = y; dz[nb++] = z;
      }
      return _label(nb,dx,dy,dz,tolerance);