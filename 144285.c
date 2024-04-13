
      static double mp_cats(_cimg_math_parser& mp) {
        const double *ptrd = &_mp_arg(1) + 1;
        const unsigned int
          sizd = (unsigned int)mp.opcode[2],
          nb_args = (unsigned int)(mp.opcode[3] - 4)/2;
        CImgList<charT> _str;
        for (unsigned int n = 0; n<nb_args; ++n) {
          const unsigned int siz = (unsigned int)mp.opcode[5 + 2*n];
          if (siz) { // Vector argument
            const double *ptrs = &_mp_arg(4 + 2*n) + 1;
            unsigned int l = 0;
            while (l<siz && ptrs[l]) ++l;
            CImg<doubleT>(ptrs,l,1,1,1,true).move_to(_str);
          } else CImg<charT>::vector((char)_mp_arg(4 + 2*n)).move_to(_str); // Scalar argument
        }
        CImg(1,1,1,1,0).move_to(_str);
        const CImg<charT> str = _str>'x';
        const unsigned int l = std::min(str._width,sizd);
        CImg<doubleT>(ptrd,l,1,1,1,true) = str.get_shared_points(0,l - 1);
        return cimg::type<double>::nan();