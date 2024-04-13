
      void self_vector_s(const unsigned int pos, const mp_func op, const unsigned int arg1) {
        const unsigned int siz = _cimg_mp_size(pos);
        if (siz>24) CImg<ulongT>::vector((ulongT)mp_self_map_vector_s,pos,siz,(ulongT)op,arg1).move_to(code);
        else {
          code.insert(siz);
          for (unsigned int k = 1; k<=siz; ++k)
            CImg<ulongT>::vector((ulongT)op,pos + k,arg1).move_to(code[code._width - 1 - siz + k]);
        }