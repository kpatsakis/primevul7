    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
    inline void swap(T1& a1, T1& b1, T2& a2, T2& b2, T3& a3, T3& b3, T4& a4, T4& b4, T5& a5, T5& b5, T6& a6, T6& b6,
                     T7& a7, T7& b7, T8& a8, T8& b8) {
      cimg::swap(a1,b1,a2,b2,a3,b3,a4,b4,a5,b5,a6,b6,a7,b7); cimg::swap(a8,b8);