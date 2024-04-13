      template<typename t>
      void operator()(const double x, const double y, const double z, const double c, t *const output) {
        mem[_cimg_mp_slot_x] = x; mem[_cimg_mp_slot_y] = y; mem[_cimg_mp_slot_z] = z; mem[_cimg_mp_slot_c] = c;
        for (p_code = code; p_code<p_code_end; ++p_code) {
          opcode._data = p_code->_data;
          const ulongT target = opcode[1];
          mem[target] = _cimg_mp_defunc(*this);
        }
        if (result_dim) {
          const double *ptrs = result + 1;
          t *ptrd = output;
          for (unsigned int k = 0; k<result_dim; ++k) *(ptrd++) = (t)*(ptrs++);
        } else *output = (t)*result;