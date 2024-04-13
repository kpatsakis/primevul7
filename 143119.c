
      static double mp_sort(_cimg_math_parser& mp) {
        double *const ptrd = &_mp_arg(1) + 1;
        const double *const ptrs = &_mp_arg(2) + 1;
        const unsigned int
          siz = (unsigned int)mp.opcode[3],
          chunk_siz = (unsigned int)mp.opcode[5];
        const bool is_increasing = (bool)_mp_arg(4);
        CImg<doubleT>(ptrd,chunk_siz,siz/chunk_siz,1,1,true) = CImg<doubleT>(ptrs,chunk_siz,siz/chunk_siz,1,1,true).
          get_sort(is_increasing,chunk_siz>1?'y':0);
        return cimg::type<double>::nan();