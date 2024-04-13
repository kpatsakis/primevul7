    template<typename T>
    inline T median(T val0, T val1, T val2, T val3, T val4) {
      T tmp = std::min(val0,val1);
      val1 = std::max(val0,val1); val0 = tmp; tmp = std::min(val3,val4); val4 = std::max(val3,val4);
      val3 = std::max(val0,tmp);  val1 = std::min(val1,val4); tmp = std::min(val1,val2); val2 = std::max(val1,val2);
      val1 = tmp; tmp = std::min(val2,val3);
      return std::max(val1,tmp);