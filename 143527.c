
      static double mp_self_map_vector_v(_cimg_math_parser& mp) { // Vector += vector
        unsigned int
          ptrd = (unsigned int)mp.opcode[1] + 1,
          siz = (unsigned int)mp.opcode[2],
          ptrs = (unsigned int)mp.opcode[4] + 1;
        mp_func op = (mp_func)mp.opcode[3];
        CImg<ulongT> l_opcode(1,4);
        l_opcode.swap(mp.opcode);
        ulongT &target = mp.opcode[1], &argument = mp.opcode[2];
        while (siz-->0) { target = ptrd++; argument = ptrs++; (*op)(mp); }
        l_opcode.swap(mp.opcode);
        return cimg::type<double>::nan();