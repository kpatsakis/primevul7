    template<typename T, typename t>
    inline T cut(const T& val, const t& val_min, const t& val_max) {
      return val<val_min?(T)val_min:val>val_max?(T)val_max:val;