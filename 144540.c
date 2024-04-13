    template<typename T>
    inline T median(T val0, T val1, T val2, T val3, T val4,
                    T val5, T val6, T val7, T val8, T val9,
                    T val10, T val11, T val12, T val13, T val14,
                    T val15, T val16, T val17, T val18, T val19,
                    T val20, T val21, T val22, T val23, T val24) {
      T tmp = std::min(val0,val1);
      val1 = std::max(val0,val1); val0 = tmp; tmp = std::min(val3,val4); val4 = std::max(val3,val4);
      val3 = tmp; tmp = std::min(val2,val4); val4 = std::max(val2,val4); val2 = std::min(tmp,val3);
      val3 = std::max(tmp,val3); tmp = std::min(val6,val7); val7 = std::max(val6,val7); val6 = tmp;
      tmp = std::min(val5,val7); val7 = std::max(val5,val7); val5 = std::min(tmp,val6); val6 = std::max(tmp,val6);
      tmp = std::min(val9,val10); val10 = std::max(val9,val10); val9 = tmp; tmp = std::min(val8,val10);
      val10 = std::max(val8,val10); val8 = std::min(tmp,val9); val9 = std::max(tmp,val9);
      tmp = std::min(val12,val13); val13 = std::max(val12,val13); val12 = tmp; tmp = std::min(val11,val13);
      val13 = std::max(val11,val13); val11 = std::min(tmp,val12); val12 = std::max(tmp,val12);
      tmp = std::min(val15,val16); val16 = std::max(val15,val16); val15 = tmp; tmp = std::min(val14,val16);
      val16 = std::max(val14,val16); val14 = std::min(tmp,val15); val15 = std::max(tmp,val15);
      tmp = std::min(val18,val19); val19 = std::max(val18,val19); val18 = tmp; tmp = std::min(val17,val19);
      val19 = std::max(val17,val19); val17 = std::min(tmp,val18); val18 = std::max(tmp,val18);
      tmp = std::min(val21,val22); val22 = std::max(val21,val22); val21 = tmp; tmp = std::min(val20,val22);
      val22 = std::max(val20,val22); val20 = std::min(tmp,val21); val21 = std::max(tmp,val21);
      tmp = std::min(val23,val24); val24 = std::max(val23,val24); val23 = tmp; tmp = std::min(val2,val5);
      val5 = std::max(val2,val5); val2 = tmp; tmp = std::min(val3,val6); val6 = std::max(val3,val6); val3 = tmp;
      tmp = std::min(val0,val6); val6 = std::max(val0,val6); val0 = std::min(tmp,val3); val3 = std::max(tmp,val3);
      tmp = std::min(val4,val7); val7 = std::max(val4,val7); val4 = tmp; tmp = std::min(val1,val7);
      val7 = std::max(val1,val7); val1 = std::min(tmp,val4); val4 = std::max(tmp,val4); tmp = std::min(val11,val14);
      val14 = std::max(val11,val14); val11 = tmp; tmp = std::min(val8,val14); val14 = std::max(val8,val14);
      val8 = std::min(tmp,val11); val11 = std::max(tmp,val11); tmp = std::min(val12,val15);
      val15 = std::max(val12,val15); val12 = tmp; tmp = std::min(val9,val15); val15 = std::max(val9,val15);
      val9 = std::min(tmp,val12); val12 = std::max(tmp,val12); tmp = std::min(val13,val16);
      val16 = std::max(val13,val16); val13 = tmp; tmp = std::min(val10,val16); val16 = std::max(val10,val16);
      val10 = std::min(tmp,val13); val13 = std::max(tmp,val13); tmp = std::min(val20,val23);
      val23 = std::max(val20,val23); val20 = tmp; tmp = std::min(val17,val23); val23 = std::max(val17,val23);
      val17 = std::min(tmp,val20); val20 = std::max(tmp,val20); tmp = std::min(val21,val24);
      val24 = std::max(val21,val24); val21 = tmp; tmp = std::min(val18,val24); val24 = std::max(val18,val24);
      val18 = std::min(tmp,val21); val21 = std::max(tmp,val21); tmp = std::min(val19,val22);
      val22 = std::max(val19,val22); val19 = tmp; val17 = std::max(val8,val17); tmp = std::min(val9,val18);
      val18 = std::max(val9,val18); val9 = tmp; tmp = std::min(val0,val18); val18 = std::max(val0,val18);
      val9 = std::max(tmp,val9); tmp = std::min(val10,val19); val19 = std::max(val10,val19); val10 = tmp;
      tmp = std::min(val1,val19); val19 = std::max(val1,val19); val1 = std::min(tmp,val10);
      val10 = std::max(tmp,val10); tmp = std::min(val11,val20); val20 = std::max(val11,val20); val11 = tmp;
      tmp = std::min(val2,val20); val20 = std::max(val2,val20); val11 = std::max(tmp,val11);
      tmp = std::min(val12,val21); val21 = std::max(val12,val21); val12 = tmp; tmp = std::min(val3,val21);
      val21 = std::max(val3,val21); val3 = std::min(tmp,val12); val12 = std::max(tmp,val12);
      tmp = std::min(val13,val22); val22 = std::max(val13,val22); val4 = std::min(val4,val22);
      val13 = std::max(val4,tmp); tmp = std::min(val4,tmp); val4 = tmp; tmp = std::min(val14,val23);
      val23 = std::max(val14,val23); val14 = tmp; tmp = std::min(val5,val23); val23 = std::max(val5,val23);
      val5 = std::min(tmp,val14); val14 = std::max(tmp,val14); tmp = std::min(val15,val24);
      val24 = std::max(val15,val24); val15 = tmp; val6 = std::min(val6,val24); tmp = std::min(val6,val15);
      val15 = std::max(val6,val15); val6 = tmp; tmp = std::min(val7,val16); val7 = std::min(tmp,val19);
      tmp = std::min(val13,val21); val15 = std::min(val15,val23); tmp = std::min(val7,tmp);
      val7 = std::min(tmp,val15); val9 = std::max(val1,val9); val11 = std::max(val3,val11);
      val17 = std::max(val5,val17); val17 = std::max(val11,val17); val17 = std::max(val9,val17);
      tmp = std::min(val4,val10); val10 = std::max(val4,val10); val4 = tmp; tmp = std::min(val6,val12);
      val12 = std::max(val6,val12); val6 = tmp; tmp = std::min(val7,val14); val14 = std::max(val7,val14);
      val7 = tmp; tmp = std::min(val4,val6); val6 = std::max(val4,val6); val7 = std::max(tmp,val7);
      tmp = std::min(val12,val14); val14 = std::max(val12,val14); val12 = tmp; val10 = std::min(val10,val14);
      tmp = std::min(val6,val7); val7 = std::max(val6,val7); val6 = tmp; tmp = std::min(val10,val12);
      val12 = std::max(val10,val12); val10 = std::max(val6,tmp); tmp = std::min(val6,tmp);
      val17 = std::max(tmp,val17); tmp = std::min(val12,val17); val17 = std::max(val12,val17); val12 = tmp;
      val7 = std::min(val7,val17); tmp = std::min(val7,val10); val10 = std::max(val7,val10); val7 = tmp;
      tmp = std::min(val12,val18); val18 = std::max(val12,val18); val12 = std::max(val7,tmp);
      val10 = std::min(val10,val18); tmp = std::min(val12,val20); val20 = std::max(val12,val20); val12 = tmp;
      tmp = std::min(val10,val20);
      return std::max(tmp,val12);