    template<typename T>
    inline T& temporary(const T&) {
      static T temp;
      return temp;