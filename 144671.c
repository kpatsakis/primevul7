
      unsigned int scalar0(const mp_func op) {
        const unsigned int pos = scalar();
        CImg<ulongT>::vector((ulongT)op,pos).move_to(code);
        return pos;