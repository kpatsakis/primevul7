
      static double mp_median(_cimg_math_parser& mp) {
        const unsigned int i_end = (unsigned int)mp.opcode[2];
        switch (i_end - 3) {
        case 1 : return _mp_arg(3);
        case 2 : return cimg::median(_mp_arg(3),_mp_arg(4));
        case 3 : return cimg::median(_mp_arg(3),_mp_arg(4),_mp_arg(5));
        case 5 : return cimg::median(_mp_arg(3),_mp_arg(4),_mp_arg(5),_mp_arg(6),_mp_arg(7));
        case 7 : return cimg::median(_mp_arg(3),_mp_arg(4),_mp_arg(5),_mp_arg(6),_mp_arg(7),_mp_arg(8),_mp_arg(9));
        case 9 : return cimg::median(_mp_arg(3),_mp_arg(4),_mp_arg(5),_mp_arg(6),_mp_arg(7),_mp_arg(8),_mp_arg(9),
                                     _mp_arg(10),_mp_arg(11));
        case 13 : return cimg::median(_mp_arg(3),_mp_arg(4),_mp_arg(5),_mp_arg(6),_mp_arg(7),_mp_arg(8),_mp_arg(9),
                                      _mp_arg(10),_mp_arg(11),_mp_arg(12),_mp_arg(13),_mp_arg(14),_mp_arg(15));
        }
        CImg<doubleT> vals(i_end - 3);
        double *p = vals.data();
        for (unsigned int i = 3; i<i_end; ++i) *(p++) = _mp_arg(i);
        return vals.median();