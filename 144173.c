
      static double mp_ext(_cimg_math_parser& mp) {
        const unsigned int nb_args = (unsigned int)(mp.opcode[2] - 3)/2;
        CImgList<charT> _str;
        CImg<charT> it;
        for (unsigned int n = 0; n<nb_args; ++n) {
          const unsigned int siz = (unsigned int)mp.opcode[4 + 2*n];
          if (siz) { // Vector argument -> string
            const double *ptr = &_mp_arg(3 + 2*n) + 1;
            unsigned int l = 0;
            while (l<siz && ptr[l]) ++l;
            CImg<doubleT>(ptr,l,1,1,1,true).move_to(_str);
          } else { // Scalar argument -> number
            it.assign(256);
            cimg_snprintf(it,it._width,"%.17g",_mp_arg(3 + 2*n));
            CImg<charT>::string(it,false,true).move_to(_str);
          }
        }
        CImg(1,1,1,1,0).move_to(_str);
        CImg<charT> str = _str>'x';
#ifdef cimg_mp_ext_function
        cimg_mp_ext_function(str);
#endif
        return cimg::type<double>::nan();