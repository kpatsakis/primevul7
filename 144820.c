    }
    inline cimg_int64 mod(const cimg_int64 x, const cimg_int64 m) {
      return x>=0?x%m:(x%m?m + x%m:0);