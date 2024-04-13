    template<typename T>
    inline int uiround(const T x) {
      return cimg::type<T>::is_float()?(int)(x + 0.5f):(int)x;