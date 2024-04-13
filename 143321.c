
    static T __cimg_blur_box_apply(T *ptr, const int N, const ulongT off,
                                   const bool boundary_conditions, const int x) {
      if (x<0) return boundary_conditions?ptr[0]:T();
      if (x>=N) return boundary_conditions?ptr[(N - 1)*off]:T();
      return ptr[x*off];