    template<typename T>
    inline T median(T val0, T val1, T val2, T val3, T val4, T val5, T val6) {
      T tmp = std::min(val0,val5);
      val5 = std::max(val0,val5); val0 = tmp; tmp = std::min(val0,val3); val3 = std::max(val0,val3); val0 = tmp;
      tmp = std::min(val1,val6); val6 = std::max(val1,val6); val1 = tmp; tmp = std::min(val2,val4);
      val4 = std::max(val2,val4); val2 = tmp; val1 = std::max(val0,val1); tmp = std::min(val3,val5);
      val5 = std::max(val3,val5); val3 = tmp; tmp = std::min(val2,val6); val6 = std::max(val2,val6);
      val3 = std::max(tmp,val3); val3 = std::min(val3,val6); tmp = std::min(val4,val5); val4 = std::max(val1,tmp);
      tmp = std::min(val1,tmp); val3 = std::max(tmp,val3);
      return std::min(val3,val4);