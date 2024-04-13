    **/
    unsigned int released_key(const unsigned int pos=0) const {
      return pos<128?_released_keys[pos]:0;