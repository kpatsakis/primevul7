
      static double mp_max(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        double val = _mp_arg(3);
        for (unsigned int i = 4; i<i_end; ++i) val = std::max(val,_mp_arg(i));
        return val;