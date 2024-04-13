    template<typename t>
    CImg<T>& _quicksort(const long indm, const long indM, CImg<t>& permutations,
                        const bool is_increasing, const bool is_permutations) {
      if (indm<indM) {
        const long mid = (indm + indM)/2;
        if (is_increasing) {
          if ((*this)[indm]>(*this)[mid]) {
            cimg::swap((*this)[indm],(*this)[mid]);
            if (is_permutations) cimg::swap(permutations[indm],permutations[mid]);
          }
          if ((*this)[mid]>(*this)[indM]) {
            cimg::swap((*this)[indM],(*this)[mid]);
            if (is_permutations) cimg::swap(permutations[indM],permutations[mid]);
          }
          if ((*this)[indm]>(*this)[mid]) {
            cimg::swap((*this)[indm],(*this)[mid]);
            if (is_permutations) cimg::swap(permutations[indm],permutations[mid]);
          }
        } else {
          if ((*this)[indm]<(*this)[mid]) {
            cimg::swap((*this)[indm],(*this)[mid]);
            if (is_permutations) cimg::swap(permutations[indm],permutations[mid]);
          }
          if ((*this)[mid]<(*this)[indM]) {
            cimg::swap((*this)[indM],(*this)[mid]);
            if (is_permutations) cimg::swap(permutations[indM],permutations[mid]);
          }
          if ((*this)[indm]<(*this)[mid]) {
            cimg::swap((*this)[indm],(*this)[mid]);
            if (is_permutations) cimg::swap(permutations[indm],permutations[mid]);
          }
        }
        if (indM - indm>=3) {
          const T pivot = (*this)[mid];
          long i = indm, j = indM;
          if (is_increasing) {
            do {
              while ((*this)[i]<pivot) ++i;
              while ((*this)[j]>pivot) --j;
              if (i<=j) {
                if (is_permutations) cimg::swap(permutations[i],permutations[j]);
                cimg::swap((*this)[i++],(*this)[j--]);
              }
            } while (i<=j);
          } else {
            do {
              while ((*this)[i]>pivot) ++i;
              while ((*this)[j]<pivot) --j;
              if (i<=j) {
                if (is_permutations) cimg::swap(permutations[i],permutations[j]);
                cimg::swap((*this)[i++],(*this)[j--]);
              }
            } while (i<=j);
          }
          if (indm<j) _quicksort(indm,j,permutations,is_increasing,is_permutations);
          if (i<indM) _quicksort(i,indM,permutations,is_increasing,is_permutations);
        }
      }
      return *this;