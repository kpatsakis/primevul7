    template<typename t, typename T>
    static void _render_resize(const T *ptrs, const unsigned int ws, const unsigned int hs,
                               t *ptrd, const unsigned int wd, const unsigned int hd) {
      typedef typename cimg::last<T,cimg_ulong>::type ulongT;
      const ulongT one = (ulongT)1;
      CImg<ulongT> off_x(wd), off_y(hd + 1);
      if (wd==ws) off_x.fill(1);
      else {
        ulongT *poff_x = off_x._data, curr = 0;
        for (unsigned int x = 0; x<wd; ++x) {
          const ulongT old = curr;
          curr = (x + one)*ws/wd;
          *(poff_x++) = curr - old;
        }
      }
      if (hd==hs) off_y.fill(ws);
      else {
        ulongT *poff_y = off_y._data, curr = 0;
        for (unsigned int y = 0; y<hd; ++y) {
          const ulongT old = curr;
          curr = (y + one)*hs/hd;
          *(poff_y++) = ws*(curr - old);
        }
        *poff_y = 0;
      }
      ulongT *poff_y = off_y._data;
      for (unsigned int y = 0; y<hd; ) {
        const T *ptr = ptrs;
        ulongT *poff_x = off_x._data;
        for (unsigned int x = 0; x<wd; ++x) { *(ptrd++) = *ptr; ptr+=*(poff_x++); }
        ++y;
        ulongT dy = *(poff_y++);
        for ( ; !dy && y<hd; std::memcpy(ptrd,ptrd - wd,sizeof(t)*wd), ++y, ptrd+=wd, dy = *(poff_y++)) {}
        ptrs+=dy;
      }