
      static double mp_print(_cimg_math_parser& mp) {
          const double val = _mp_arg(1);
          const bool print_char = (bool)mp.opcode[3];
          cimg_pragma_openmp(critical(mp_print))
          {
            CImg<charT> _expr(mp.opcode[2] - 4);
            const ulongT *ptrs = mp.opcode._data + 4;
            cimg_for(_expr,ptrd,char) *ptrd = (char)*(ptrs++);
            cimg::strellipsize(_expr);
            cimg::mutex(6);
            if (print_char)
              std::fprintf(cimg::output(),"\n[" cimg_appname "_math_parser] %s = %g = '%c'",_expr._data,val,(int)val);
            else
              std::fprintf(cimg::output(),"\n[" cimg_appname "_math_parser] %s = %g",_expr._data,val);
            std::fflush(cimg::output());
            cimg::mutex(6,0);
          }
          return val;