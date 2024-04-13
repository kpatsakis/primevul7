
      static double mp_vector_reverse(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const double *const ptrs = &_mp_arg(2) + 1;
        const unsigned int p1 = (unsigned int)mp.opcode[3];
        CImg<doubleT>(ptrd,p1,1,1,1,true) = CImg<doubleT>(ptrs,p1,1,1,1,true).get_mirror('x');
        return cimg::type<double>::nan();