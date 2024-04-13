
      static double mp_norm0(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        switch (i_end - 3) {
        case 1 : return _mp_arg(3)!=0;
        case 2 : return (_mp_arg(3)!=0) + (_mp_arg(4)!=0);
        }
        double res = 0;
        for (unsigned int i = 3; i<i_end; ++i)
          res+=_mp_arg(i)==0?0:1;
        return res;