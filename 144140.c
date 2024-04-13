
      static double mp_norm1(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        switch (i_end - 3) {
        case 1 : return cimg::abs(_mp_arg(3));
        case 2 : return cimg::abs(_mp_arg(3)) + cimg::abs(_mp_arg(4));
        }
        double res = 0;
        for (unsigned int i = 3; i<i_end; ++i)
          res+=cimg::abs(_mp_arg(i));
        return res;