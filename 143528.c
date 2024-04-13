
      static double mp_dot(_cimg_math_parser& mp) {
        const unsigned int siz = (unsigned int)mp.opcode[4];
        return CImg<doubleT>(&_mp_arg(2) + 1,1,siz,1,1,true).
          dot(CImg<doubleT>(&_mp_arg(3) + 1,1,siz,1,1,true));