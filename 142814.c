
      static double mp_vector_map_v(_cimg_math_parser& mp) { // Operator(vector)
        unsigned int
          siz = (unsigned int)mp.opcode[2],
          ptrs = (unsigned int)mp.opcode[4] + 1;
        double *ptrd = &_mp_arg(1) + 1;
        mp_func op = (mp_func)mp.opcode[3];
        CImg<ulongT> l_opcode(1,3);
        l_opcode.swap(mp.opcode);
        ulongT &argument = mp.opcode[2];
        while (siz-->0) { argument = ptrs++; *(ptrd++) = (*op)(mp); }
        l_opcode.swap(mp.opcode);
        return cimg::type<double>::nan();