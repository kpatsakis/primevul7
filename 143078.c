    template<typename T>
    inline T sign(const T& x) {
      return (T)(cimg::type<T>::is_nan(x)?0:x<0?-1:x>0);