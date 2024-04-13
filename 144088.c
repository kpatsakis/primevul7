
      static double mp_shift(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const double *const ptrs = &_mp_arg(2) + 1;
        const unsigned int siz = (unsigned int)mp.opcode[3];
        const int
          shift = (int)_mp_arg(4),
          boundary_conditions = (int)_mp_arg(5);
        CImg<doubleT>(ptrd,siz,1,1,1,true) = CImg<doubleT>(ptrs,siz,1,1,1,true).shift(shift,0,0,0,boundary_conditions);
        return cimg::type<double>::nan();