
      static double mp_matrix_inv(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const double *ptr1 = &_mp_arg(2) + 1;
        const unsigned int k = (unsigned int)mp.opcode[3];
        CImg<doubleT>(ptrd,k,k,1,1,true) = CImg<doubleT>(ptr1,k,k,1,1,true).get_invert();
        return cimg::type<double>::nan();