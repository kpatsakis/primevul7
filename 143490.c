
      static double mp_vector_map_vv(_cimg_math_parser& mp) { // Operator(vector,vector)
        unsigned int
          siz = (unsigned int)mp.opcode[2],
          ptrs1 = (unsigned int)mp.opcode[4] + 1,
          ptrs2 = (unsigned int)mp.opcode[5] + 1;
        double *ptrd = &_mp_arg(1) + 1;
        mp_func op = (mp_func)mp.opcode[3];
        CImg<ulongT> l_opcode(1,4);
        l_opcode.swap(mp.opcode);
        ulongT &argument1 = mp.opcode[2], &argument2 = mp.opcode[3];
        while (siz-->0) { argument1 = ptrs1++; argument2 = ptrs2++; *(ptrd++) = (*op)(mp); }
        l_opcode.swap(mp.opcode);
        return cimg::type<double>::nan();