    **/
    bool is_key_sequence(const unsigned int *const keycodes_sequence, const unsigned int length,
                         const bool remove_sequence=false) {
      if (keycodes_sequence && length) {
        const unsigned int
          *const ps_end = keycodes_sequence + length - 1,
          *const pk_end = (unsigned int*)_keys + 1 + 128 - length,
          k = *ps_end;
        for (unsigned int *pk = (unsigned int*)_keys; pk<pk_end; ) {
          if (*(pk++)==k) {
            bool res = true;
            const unsigned int *ps = ps_end, *pk2 = pk;
            for (unsigned int i = 1; i<length; ++i) res = (*(--ps)==*(pk2++));
            if (res) {
              if (remove_sequence) std::memset((void*)(pk - 1),0,sizeof(unsigned int)*length);
              return true;
            }
          }
        }
      }
      return false;