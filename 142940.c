
      static double mp_string_init(_cimg_math_parser& mp) {
        const char *ptrs = (char*)&mp.opcode[3];
        unsigned int
          ptrd = (unsigned int)mp.opcode[1] + 1,
          siz = (unsigned int)mp.opcode[2];
        while (siz-->0) mp.mem[ptrd++] = (double)*(ptrs++);
        return cimg::type<double>::nan();