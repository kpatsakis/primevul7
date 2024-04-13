
      static double mp_vector_init(_cimg_math_parser& mp) {
        unsigned int
          ptrs = 4U,
          ptrd = (unsigned int)mp.opcode[1] + 1,
          siz = (unsigned int)mp.opcode[3];
        switch (mp.opcode[2] - 4) {
        case 0 : std::memset(mp.mem._data + ptrd,0,siz*sizeof(double)); break; // 0 values given
        case 1 : { const double val = _mp_arg(ptrs); while (siz-->0) mp.mem[ptrd++] = val; } break;
        default : while (siz-->0) { mp.mem[ptrd++] = _mp_arg(ptrs++); if (ptrs>=mp.opcode[2]) ptrs = 4U; }
        }
        return cimg::type<double>::nan();