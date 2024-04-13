
      static double mp_for(_cimg_math_parser& mp) {
        const ulongT
          mem_body = mp.opcode[1],
          mem_cond = mp.opcode[3];
        const CImg<ulongT>
          *const p_init = ++mp.p_code,
          *const p_cond = p_init + mp.opcode[4],
          *const p_body = p_cond + mp.opcode[5],
          *const p_post = p_body + mp.opcode[6],
          *const p_end = p_post + mp.opcode[7];
        const unsigned int vsiz = (unsigned int)mp.opcode[2];
        bool is_cond = false;
        if (mp.opcode[8]) { // Set default value for result and condition if necessary
          if (vsiz) CImg<doubleT>(&mp.mem[mem_body] + 1,vsiz,1,1,1,true).fill(cimg::type<double>::nan());
          else mp.mem[mem_body] = cimg::type<double>::nan();
        }
        if (mp.opcode[9]) mp.mem[mem_cond] = 0;
        const unsigned int _break_type = mp.break_type;
        mp.break_type = 0;

        for (mp.p_code = p_init; mp.p_code<p_cond; ++mp.p_code) { // Evaluate init
          mp.opcode._data = mp.p_code->_data;
          const ulongT target = mp.opcode[1];
          mp.mem[target] = _cimg_mp_defunc(mp);
        }

        if (!mp.break_type) do {
            for (mp.p_code = p_cond; mp.p_code<p_body; ++mp.p_code) { // Evaluate condition
              mp.opcode._data = mp.p_code->_data;
              const ulongT target = mp.opcode[1];
              mp.mem[target] = _cimg_mp_defunc(mp);
            }
            if (mp.break_type==1) break;

            is_cond = (bool)mp.mem[mem_cond];
            if (is_cond && !mp.break_type) {
              for (mp.p_code = p_body; mp.p_code<p_post; ++mp.p_code) { // Evaluate body
                mp.opcode._data = mp.p_code->_data;
                const ulongT target = mp.opcode[1];
                mp.mem[target] = _cimg_mp_defunc(mp);
              }
              if (mp.break_type==1) break; else if (mp.break_type==2) mp.break_type = 0;

              for (mp.p_code = p_post; mp.p_code<p_end; ++mp.p_code) { // Evaluate post-code
                mp.opcode._data = mp.p_code->_data;
                const ulongT target = mp.opcode[1];
                mp.mem[target] = _cimg_mp_defunc(mp);
              }
              if (mp.break_type==1) break; else if (mp.break_type==2) mp.break_type = 0;
            }
          } while (is_cond);

        mp.break_type = _break_type;
        mp.p_code = p_end - 1;
        return mp.mem[mem_body];