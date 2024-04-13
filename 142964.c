
      static double mp_trace(_cimg_math_parser& mp) {
        const double *ptrs = &_mp_arg(2) + 1;
        const unsigned int k = (unsigned int)mp.opcode[3];
        return CImg<doubleT>(ptrs,k,k,1,1,true).trace();