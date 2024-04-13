
      static double mp_logical_and(_cimg_math_parser& mp) {
        const bool val_left = (bool)_mp_arg(2);
        const CImg<ulongT> *const p_end = ++mp.p_code + mp.opcode[4];
        if (!val_left) { mp.p_code = p_end - 1; return 0; }
        const ulongT mem_right = mp.opcode[3];
        for ( ; mp.p_code<p_end; ++mp.p_code) {
          mp.opcode._data = mp.p_code->_data;
          const ulongT target = mp.opcode[1];
          mp.mem[target] = _cimg_mp_defunc(mp);
        }
        --mp.p_code;
        return (double)(bool)mp.mem[mem_right];