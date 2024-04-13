    template<typename t>
    CImg<T>& set_tensor_at(const CImg<t>& ten, const unsigned int x=0, const unsigned int y=0, const unsigned int z=0) {
      T *ptrd = data(x,y,z,0);
      const ulongT siz = (ulongT)_width*_height*_depth;
      if (ten._height==2) {
        *ptrd = (T)ten[0]; ptrd+=siz;
        *ptrd = (T)ten[1]; ptrd+=siz;
        *ptrd = (T)ten[3];
      }
      else {
        *ptrd = (T)ten[0]; ptrd+=siz;
        *ptrd = (T)ten[1]; ptrd+=siz;
        *ptrd = (T)ten[2]; ptrd+=siz;
        *ptrd = (T)ten[4]; ptrd+=siz;
        *ptrd = (T)ten[5]; ptrd+=siz;
        *ptrd = (T)ten[8];
      }
      return *this;