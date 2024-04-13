
      unsigned int vector_copy(const unsigned int arg) { // Insert new copy of specified vector in memory
        const unsigned int
          siz = _cimg_mp_size(arg),
          pos = vector(siz);
        CImg<ulongT>::vector((ulongT)mp_vector_copy,pos,arg,siz).move_to(code);
        return pos;