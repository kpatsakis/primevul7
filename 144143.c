    template<typename T>
    inline T median(T val0, T val1, T val2) {
      return std::max(std::min(val0,val1),std::min(val2,std::max(val0,val1)));