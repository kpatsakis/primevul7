
      static double mp_critical(_cimg_math_parser& mp) {
        const double res = _mp_arg(1);
        cimg_pragma_openmp(critical(mp_critical))
        {
          for (const CImg<ulongT> *const p_end = ++mp.p_code + mp.opcode[2];
               mp.p_code<p_end; ++mp.p_code) { // Evaluate body
            mp.opcode._data = mp.p_code->_data;
            const ulongT target = mp.opcode[1];
            mp.mem[target] = _cimg_mp_defunc(mp);
          }
        }
        --mp.p_code;
        return res;