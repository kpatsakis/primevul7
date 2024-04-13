
      static double mp_eye(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const unsigned int k = (unsigned int)mp.opcode[2];
        CImg<doubleT>(ptrd,k,k,1,1,true).identity_matrix();
        return cimg::type<double>::nan();