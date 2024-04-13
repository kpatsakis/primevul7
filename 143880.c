
      static double mp_vector_set_off(_cimg_math_parser& mp) {
        const unsigned int
          ptr = (unsigned int)mp.opcode[2] + 1,
          siz = (unsigned int)mp.opcode[3];
        const int off = (int)_mp_arg(4);
        if (off>=0 && off<(int)siz) mp.mem[ptr + off] = _mp_arg(5);
        return _mp_arg(5);