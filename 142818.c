
      static double mp_find(_cimg_math_parser& mp) {
        const bool is_forward = (bool)_mp_arg(5);
        const ulongT siz = (ulongT)mp.opcode[3];
        longT ind = (longT)(mp.opcode[6]!=_cimg_mp_slot_nan?_mp_arg(6):is_forward?0:siz - 1);
        if (ind<0 || ind>=(longT)siz) return -1.;
        const double
          *const ptrb = &_mp_arg(2) + 1,
          *const ptre = ptrb + siz,
          val = _mp_arg(4),
          *ptr = ptrb + ind;

        // Forward search
        if (is_forward) {
          while (ptr<ptre && *ptr!=val) ++ptr;
          return ptr==ptre?-1.:(double)(ptr - ptrb);
        }

        // Backward search.
        while (ptr>=ptrb && *ptr!=val) --ptr;
        return ptr<ptrb?-1.:(double)(ptr - ptrb);