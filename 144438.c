
      static double mp_matrix_pseudoinv(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const double *ptr1 = &_mp_arg(2) + 1;
        const unsigned int
          k = (unsigned int)mp.opcode[3],
          l = (unsigned int)mp.opcode[4];
        CImg<doubleT>(ptrd,l,k,1,1,true) = CImg<doubleT>(ptr1,k,l,1,1,true).get_pseudoinvert();
        return cimg::type<double>::nan();