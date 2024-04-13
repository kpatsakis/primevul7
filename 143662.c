
      unsigned int vector3_vss(const mp_func op, const unsigned int arg1, const unsigned int arg2,
                               const unsigned int arg3) {
        const unsigned int
          siz = _cimg_mp_size(arg1),
          pos = is_comp_vector(arg1)?arg1:vector(siz);
        if (siz>24) CImg<ulongT>::vector((ulongT)mp_vector_map_vss,pos,siz,(ulongT)op,arg1,arg2,arg3).move_to(code);
        else {
          code.insert(siz);
          for (unsigned int k = 1; k<=siz; ++k)
            CImg<ulongT>::vector((ulongT)op,pos + k,arg1 + k,arg2,arg3).move_to(code[code._width - 1 - siz + k]);
        }
        return pos;