      // Evaluation procedure.
      double operator()(const double x, const double y, const double z, const double c) {
        mem[_cimg_mp_slot_x] = x; mem[_cimg_mp_slot_y] = y; mem[_cimg_mp_slot_z] = z; mem[_cimg_mp_slot_c] = c;
        for (p_code = code; p_code<p_code_end; ++p_code) {
          opcode._data = p_code->_data;
          const ulongT target = opcode[1];
          mem[target] = _cimg_mp_defunc(*this);
        }
        return *result;