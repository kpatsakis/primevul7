
      static double mp_diag(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2], siz = mp.opcode[2] - 3;
        double *ptrd = &_mp_arg(1) + 1;
        std::memset(ptrd,0,siz*siz*sizeof(double));
        for (unsigned int i = 3; i<i_end; ++i) { *(ptrd++) = _mp_arg(i); ptrd+=siz; }
        return cimg::type<double>::nan();