
      static double mp_norm2(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        switch (i_end - 3) {
        case 1 : return cimg::abs(_mp_arg(3));
        case 2 : return cimg::_hypot(_mp_arg(3),_mp_arg(4));
        }
        double res = 0;
        for (unsigned int i = 3; i<i_end; ++i)
          res+=cimg::sqr(_mp_arg(i));
        return std::sqrt(res);