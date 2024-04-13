    **/
    T kth_smallest(const ulongT k) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "kth_smallest(): Empty instance.",
                                    cimg_instance);
      CImg<T> arr(*this,false);
      ulongT l = 0, ir = size() - 1;
      for ( ; ; ) {
        if (ir<=l + 1) {
          if (ir==l + 1 && arr[ir]<arr[l]) cimg::swap(arr[l],arr[ir]);
          return arr[k];
        } else {
          const ulongT mid = (l + ir)>>1;
          cimg::swap(arr[mid],arr[l + 1]);
          if (arr[l]>arr[ir]) cimg::swap(arr[l],arr[ir]);
          if (arr[l + 1]>arr[ir]) cimg::swap(arr[l + 1],arr[ir]);
          if (arr[l]>arr[l + 1]) cimg::swap(arr[l],arr[l + 1]);
          ulongT i = l + 1, j = ir;
          const T pivot = arr[l + 1];
          for ( ; ; ) {
            do ++i; while (arr[i]<pivot);
            do --j; while (arr[j]>pivot);
            if (j<i) break;
            cimg::swap(arr[i],arr[j]);
          }
          arr[l + 1] = arr[j];
          arr[j] = pivot;
          if (j>=k) ir = j - 1;
          if (j<=k) l = i;
        }
      }