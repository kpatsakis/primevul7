
      static double mp_vtos(_cimg_math_parser& mp) {
        double *ptrd = &_mp_arg(1) + 1;
        const unsigned int
          sizd = (unsigned int)mp.opcode[2],
          sizs = (unsigned int)mp.opcode[4];
        const int nb_digits = (int)_mp_arg(5);
        CImg<charT> format(8);
        switch (nb_digits) {
        case -1 : std::strcpy(format,"%g"); break;
        case 0 : std::strcpy(format,"%.17g"); break;
        default : cimg_snprintf(format,format._width,"%%.%dg",nb_digits);
        }
        CImg<charT> str;
        if (sizs) { // Vector expression
          const double *ptrs = &_mp_arg(3) + 1;
          CImg<doubleT>(ptrs,sizs,1,1,1,true).value_string(',',sizd + 1,format).move_to(str);
        } else { // Scalar expression
          str.assign(sizd + 1);
          cimg_snprintf(str,sizd + 1,format,_mp_arg(3));
        }
        const unsigned int l = std::min(sizd,(unsigned int)std::strlen(str) + 1);
        CImg<doubleT>(ptrd,l,1,1,1,true) = str.get_shared_points(0,l - 1);
        return cimg::type<double>::nan();