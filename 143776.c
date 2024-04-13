
      static double mp_if(_cimg_math_parser& mp) {
        const bool is_cond = (bool)_mp_arg(2);
        const ulongT
          mem_left = mp.opcode[3],
          mem_right = mp.opcode[4];
        const CImg<ulongT>
          *const p_right = ++mp.p_code + mp.opcode[5],
          *const p_end = p_right + mp.opcode[6];
        const unsigned int vtarget = (unsigned int)mp.opcode[1], vsiz = (unsigned int)mp.opcode[7];
        if (is_cond) for ( ; mp.p_code<p_right; ++mp.p_code) {
            mp.opcode._data = mp.p_code->_data;
            const ulongT target = mp.opcode[1];
            mp.mem[target] = _cimg_mp_defunc(mp);
          }
        else for (mp.p_code = p_right; mp.p_code<p_end; ++mp.p_code) {
            mp.opcode._data = mp.p_code->_data;
            const ulongT target = mp.opcode[1];
            mp.mem[target] = _cimg_mp_defunc(mp);
          }
        if (mp.p_code==mp.p_break) --mp.p_code;
        else mp.p_code = p_end - 1;
        if (vsiz) std::memcpy(&mp.mem[vtarget] + 1,&mp.mem[is_cond?mem_left:mem_right] + 1,sizeof(double)*vsiz);
        return mp.mem[is_cond?mem_left:mem_right];