    // [internal] Return a 40x38 color logo of a 'danger' item.
    static CImg<T> _logo40x38() {
      CImg<T> res(40,38,1,3);
      const unsigned char *ptrs = cimg::logo40x38;
      T *ptr1 = res.data(0,0,0,0), *ptr2 = res.data(0,0,0,1), *ptr3 = res.data(0,0,0,2);
      for (ulongT off = 0; off<(ulongT)res._width*res._height;) {
        const unsigned char n = *(ptrs++), r = *(ptrs++), g = *(ptrs++), b = *(ptrs++);
        for (unsigned int l = 0; l<n; ++off, ++l) { *(ptr1++) = (T)r; *(ptr2++) = (T)g; *(ptr3++) = (T)b; }
      }
      return res;