
      static double mp_vector_map_vss(_cimg_math_parser& mp) { // Operator(vector,scalar,scalar)
        unsigned int
          siz = (unsigned int)mp.opcode[2],
          ptrs = (unsigned int)mp.opcode[4] + 1;
        double *ptrd = &_mp_arg(1) + 1;
        mp_func op = (mp_func)mp.opcode[3];
        CImg<ulongT> l_opcode(1,5);
        l_opcode[3] = mp.opcode[5]; // Scalar argument2
        l_opcode[4] = mp.opcode[6]; // Scalar argument3
        l_opcode.swap(mp.opcode);
        ulongT &argument1 = mp.opcode[2];
        while (siz-->0) { argument1 = ptrs++; *(ptrd++) = (*op)(mp); }
        l_opcode.swap(mp.opcode);
        return cimg::type<double>::nan();