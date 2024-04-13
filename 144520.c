
      static double mp_matrix_mul(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const double
          *ptr1 = &_mp_arg(2) + 1,
          *ptr2 = &_mp_arg(3) + 1;
        const unsigned int
          k = (unsigned int)mp.opcode[4],
          l = (unsigned int)mp.opcode[5],
          m = (unsigned int)mp.opcode[6];
        CImg<doubleT>(ptrd,m,k,1,1,true) = CImg<doubleT>(ptr1,l,k,1,1,true)*CImg<doubleT>(ptr2,m,l,1,1,true);
        return cimg::type<double>::nan();