
      static double mp_vector_map_sv(_cimg_math_parser& mp) { // Operator(scalar,vector)
        unsigned int
          siz = (unsigned int)mp.opcode[2],
          ptrs = (unsigned int)mp.opcode[5] + 1;
        double *ptrd = &_mp_arg(1) + 1;
        mp_func op = (mp_func)mp.opcode[3];
        CImg<ulongT> l_opcode(4);
        l_opcode[2] = mp.opcode[4]; // Scalar argument1
        l_opcode.swap(mp.opcode);
        ulongT &argument2 = mp.opcode[3];
        while (siz-->0) { argument2 = ptrs++; *(ptrd++) = (*op)(mp); }
        l_opcode.swap(mp.opcode);
        return cimg::type<double>::nan();