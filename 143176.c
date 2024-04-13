    template<typename T>
    inline T median(T val0, T val1, T val2, T val3, T val4, T val5, T val6, T val7, T val8) {
      T tmp = std::min(val1,val2);
      val2 = std::max(val1,val2); val1 = tmp; tmp = std::min(val4,val5);
      val5 = std::max(val4,val5); val4 = tmp; tmp = std::min(val7,val8);
      val8 = std::max(val7,val8); val7 = tmp; tmp = std::min(val0,val1);
      val1 = std::max(val0,val1); val0 = tmp; tmp = std::min(val3,val4);
      val4 = std::max(val3,val4); val3 = tmp; tmp = std::min(val6,val7);
      val7 = std::max(val6,val7); val6 = tmp; tmp = std::min(val1,val2);
      val2 = std::max(val1,val2); val1 = tmp; tmp = std::min(val4,val5);
      val5 = std::max(val4,val5); val4 = tmp; tmp = std::min(val7,val8);
      val8 = std::max(val7,val8); val3 = std::max(val0,val3); val5 = std::min(val5,val8);
      val7 = std::max(val4,tmp); tmp = std::min(val4,tmp); val6 = std::max(val3,val6);
      val4 = std::max(val1,tmp); val2 = std::min(val2,val5); val4 = std::min(val4,val7);
      tmp = std::min(val4,val2); val2 = std::max(val4,val2); val4 = std::max(val6,tmp);
      return std::min(val4,val2);