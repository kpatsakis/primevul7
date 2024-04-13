    template<typename T>
    inline T median(T val0, T val1, T val2, T val3, T val4, T val5, T val6, T val7, T val8, T val9, T val10, T val11,
                    T val12) {
      T tmp = std::min(val1,val7);
      val7 = std::max(val1,val7); val1 = tmp; tmp = std::min(val9,val11); val11 = std::max(val9,val11); val9 = tmp;
      tmp = std::min(val3,val4);  val4 = std::max(val3,val4); val3 = tmp; tmp = std::min(val5,val8);
      val8 = std::max(val5,val8); val5 = tmp; tmp = std::min(val0,val12); val12 = std::max(val0,val12);
      val0 = tmp; tmp = std::min(val2,val6); val6 = std::max(val2,val6); val2 = tmp; tmp = std::min(val0,val1);
      val1 = std::max(val0,val1); val0 = tmp; tmp = std::min(val2,val3); val3 = std::max(val2,val3); val2 = tmp;
      tmp = std::min(val4,val6);  val6 = std::max(val4,val6); val4 = tmp; tmp = std::min(val8,val11);
      val11 = std::max(val8,val11); val8 = tmp; tmp = std::min(val7,val12); val12 = std::max(val7,val12); val7 = tmp;
      tmp = std::min(val5,val9); val9 = std::max(val5,val9); val5 = tmp; tmp = std::min(val0,val2);
      val2 = std::max(val0,val2); val0 = tmp; tmp = std::min(val3,val7); val7 = std::max(val3,val7); val3 = tmp;
      tmp = std::min(val10,val11); val11 = std::max(val10,val11); val10 = tmp; tmp = std::min(val1,val4);
      val4 = std::max(val1,val4); val1 = tmp; tmp = std::min(val6,val12); val12 = std::max(val6,val12); val6 = tmp;
      tmp = std::min(val7,val8); val8 = std::max(val7,val8); val7 = tmp; val11 = std::min(val11,val12);
      tmp = std::min(val4,val9); val9 = std::max(val4,val9); val4 = tmp; tmp = std::min(val6,val10);
      val10 = std::max(val6,val10); val6 = tmp; tmp = std::min(val3,val4); val4 = std::max(val3,val4); val3 = tmp;
      tmp = std::min(val5,val6); val6 = std::max(val5,val6); val5 = tmp; val8 = std::min(val8,val9);
      val10 = std::min(val10,val11); tmp = std::min(val1,val7); val7 = std::max(val1,val7); val1 = tmp;
      tmp = std::min(val2,val6); val6 = std::max(val2,val6); val2 = tmp; val3 = std::max(val1,val3);
      tmp = std::min(val4,val7); val7 = std::max(val4,val7); val4 = tmp; val8 = std::min(val8,val10);
      val5 = std::max(val0,val5); val5 = std::max(val2,val5); tmp = std::min(val6,val8); val8 = std::max(val6,val8);
      val5 = std::max(val3,val5); val7 = std::min(val7,val8); val6 = std::max(val4,tmp); tmp = std::min(val4,tmp);
      val5 = std::max(tmp,val5); val6 = std::min(val6,val7);
      return std::max(val5,val6);