
      static double mp_date(_cimg_math_parser& mp) {
        const unsigned int
          siz_out = (unsigned int)mp.opcode[2],
          siz_arg1 = (unsigned int)mp.opcode[4],
          siz_arg2 = (unsigned int)mp.opcode[6];
        double *ptr_out = &_mp_arg(1) + (siz_out?1:0);
        const double
          *ptr_arg1 = siz_arg1==~0U?0:&_mp_arg(3) + (siz_arg1?1:0),
          *ptr_arg2 = siz_arg2==~0U?0:&_mp_arg(5) + 1;

        if (!ptr_arg2) { // No filename specified
          if (!siz_arg1) return cimg::date((unsigned int)*ptr_arg1);
          if (siz_arg1==~0U) for (unsigned int k = 0; k<siz_out; ++k) ptr_out[k] = k;
          else for (unsigned int k = 0; k<siz_out; ++k) ptr_out[k] = ptr_arg1[k];
          cimg::date(ptr_out,siz_out);
          return cimg::type<double>::nan();
        }

        // Filename specified.
        CImg<charT> ss(siz_arg2 + 1);
        cimg_forX(ss,i) ss[i] = (char)ptr_arg2[i];
        ss.back() = 0;
        if (!siz_arg1) return cimg::fdate(ss,(unsigned int)*ptr_arg1);
        for (unsigned int k = 0; k<siz_out; ++k) ptr_out[k] = ptr_arg1[k];
        cimg::fdate(ss,ptr_out,siz_out);
        return cimg::type<double>::nan();