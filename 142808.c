    **/
    unsigned int key(const unsigned int pos=0) const {
      return pos<128?_keys[pos]:0;