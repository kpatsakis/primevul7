
      static double mp_list_find(_cimg_math_parser& mp) {
        const unsigned int
          indi = (unsigned int)cimg::mod((int)_mp_arg(2),mp.listin.width());
        const CImg<T> &img = mp.listin[indi];
        const bool is_forward = (bool)_mp_arg(4);
        const ulongT siz = (ulongT)img.size();
        longT ind = (longT)(mp.opcode[5]!=_cimg_mp_slot_nan?_mp_arg(5):is_forward?0:siz - 1);
        if (ind<0 || ind>=(longT)siz) return -1.;
        const T
          *const ptrb = img.data(),
          *const ptre = img.end(),
          *ptr = ptrb + ind;
        const double val = _mp_arg(3);

        // Forward search
        if (is_forward) {
          while (ptr<ptre && (double)*ptr!=val) ++ptr;
          return ptr==ptre?-1.:(double)(ptr - ptrb);
        }

        // Backward search.
        while (ptr>=ptrb && (double)*ptr!=val) --ptr;
        return ptr<ptrb?-1.:(double)(ptr - ptrb);