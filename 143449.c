
      static double mp_arg(_cimg_math_parser& mp) {
        const int _ind = (int)_mp_arg(4);
        const unsigned int
          nb_args = (unsigned int)mp.opcode[2] - 4,
          ind = _ind<0?_ind + nb_args:(unsigned int)_ind,
          siz = (unsigned int)mp.opcode[3];
        if (siz>0) {
          if (ind>=nb_args) std::memset(&_mp_arg(1) + 1,0,siz*sizeof(double));
          else std::memcpy(&_mp_arg(1) + 1,&_mp_arg(ind + 4) + 1,siz*sizeof(double));
          return cimg::type<double>::nan();
        }
        if (ind>=nb_args) return 0;
        return _mp_arg(ind + 4);