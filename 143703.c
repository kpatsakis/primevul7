     **/
    T median() const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "median(): Empty instance.",
                                    cimg_instance);
      const ulongT s = size();
      switch (s) {
      case 1 : return _data[0];
      case 2 : return cimg::median(_data[0],_data[1]);
      case 3 : return cimg::median(_data[0],_data[1],_data[2]);
      case 5 : return cimg::median(_data[0],_data[1],_data[2],_data[3],_data[4]);
      case 7 : return cimg::median(_data[0],_data[1],_data[2],_data[3],_data[4],_data[5],_data[6]);
      case 9 : return cimg::median(_data[0],_data[1],_data[2],_data[3],_data[4],_data[5],_data[6],_data[7],_data[8]);
      case 13 : return cimg::median(_data[0],_data[1],_data[2],_data[3],_data[4],_data[5],_data[6],_data[7],_data[8],
                                    _data[9],_data[10],_data[11],_data[12]);
      }
      const T res = kth_smallest(s>>1);
      return (s%2)?res:(T)((res + kth_smallest((s>>1) - 1))/2);