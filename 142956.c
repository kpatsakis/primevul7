
      static double mp_norminf(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        switch (i_end - 3) {
        case 1 : return cimg::abs(_mp_arg(3));
        case 2 : return std::max(cimg::abs(_mp_arg(3)),cimg::abs(_mp_arg(4)));
        }
        double res = 0;
        for (unsigned int i = 3; i<i_end; ++i) {
          const double val = cimg::abs(_mp_arg(i));
          if (val>res) res = val;
        }
        return res;