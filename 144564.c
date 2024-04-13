
      static double mp_list_Ioff(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const unsigned int
          ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.listin.width()),
          boundary_conditions = (unsigned int)_mp_arg(4),
          vsiz = (unsigned int)mp.opcode[5];
        const CImg<T> &img = mp.listin[ind];
        const longT
          off = (longT)_mp_arg(3),
          whd = (longT)img.width()*img.height()*img.depth();
        const T *ptrs;
        if (off>=0 && off<whd) {
          ptrs = &img[off];
          cimg_for_inC(img,0,vsiz - 1,c) { *(ptrd++) = *ptrs; ptrs+=whd; }
          return cimg::type<double>::nan();
        }
        if (img._data) switch (boundary_conditions) {
          case 3 : { // Mirror
            const longT whd2 = 2*whd, moff = cimg::mod(off,whd2);
            ptrs = &img[moff<whd?moff:whd2 - moff - 1];
            cimg_for_inC(img,0,vsiz - 1,c) { *(ptrd++) = *ptrs; ptrs+=whd; }
            return cimg::type<double>::nan();
          }
          case 2 : // Periodic
            ptrs = &img[cimg::mod(off,whd)];
            cimg_for_inC(img,0,vsiz - 1,c) { *(ptrd++) = *ptrs; ptrs+=whd; }
            return cimg::type<double>::nan();
          case 1 : // Neumann
            ptrs = off<0?&img[0]:&img[whd - 1];
            cimg_for_inC(img,0,vsiz - 1,c) { *(ptrd++) = *ptrs; ptrs+=whd; }
            return cimg::type<double>::nan();
          default : // Dirichlet
            std::memset(ptrd,0,vsiz*sizeof(double));
            return cimg::type<double>::nan();
          }
        std::memset(ptrd,0,vsiz*sizeof(double));
        return cimg::type<double>::nan();