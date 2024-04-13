
      static double mp_argmax(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        double val = _mp_arg(3);
        unsigned int argval = 0;
        for (unsigned int i = 4; i<i_end; ++i) {
          const double _val = _mp_arg(i);
          if (_val>val) { val = _val; argval = i - 3; }
        }
        return (double)argval;