    template<typename T>
    inline void invert_endianness(T* const buffer, const cimg_ulong size) {
      if (size) switch (sizeof(T)) {
        case 1 : break;
        case 2 : {
          for (unsigned short *ptr = (unsigned short*)buffer + size; ptr>(unsigned short*)buffer; ) {
            const unsigned short val = *(--ptr);
            *ptr = (unsigned short)((val>>8) | ((val<<8)));
          }
        } break;
        case 4 : {
          for (unsigned int *ptr = (unsigned int*)buffer + size; ptr>(unsigned int*)buffer; ) {
            const unsigned int val = *(--ptr);
            *ptr = (val>>24) | ((val>>8)&0xff00) | ((val<<8)&0xff0000) | (val<<24);
          }
        } break;
        case 8 : {
          const cimg_uint64
            m0 = (cimg_uint64)0xff, m1 = m0<<8, m2 = m0<<16, m3 = m0<<24,
            m4 = m0<<32, m5 = m0<<40, m6 = m0<<48, m7 = m0<<56;
          for (cimg_uint64 *ptr = (cimg_uint64*)buffer + size; ptr>(cimg_uint64*)buffer; ) {
            const cimg_uint64 val = *(--ptr);
            *ptr =  (((val&m7)>>56) | ((val&m6)>>40) | ((val&m5)>>24) | ((val&m4)>>8) |
                     ((val&m3)<<8) |((val&m2)<<24) | ((val&m1)<<40) | ((val&m0)<<56));
          }
        } break;
        default : {
          for (T* ptr = buffer + size; ptr>buffer; ) {
            unsigned char *pb = (unsigned char*)(--ptr), *pe = pb + sizeof(T);
            for (int i = 0; i<(int)sizeof(T)/2; ++i) swap(*(pb++),*(--pe));
          }
        }
        }