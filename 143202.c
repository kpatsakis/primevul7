
    CImg<T>& _priority_queue_remove(unsigned int& siz) {
      (*this)(0,0) = (*this)(--siz,0);
      (*this)(0,1) = (*this)(siz,1);
      (*this)(0,2) = (*this)(siz,2);
      (*this)(0,3) = (*this)(siz,3);
      const float value = (*this)(0,0);
      unsigned int pos = 0, swap = 0;
      do {
        const unsigned int left = 2*pos + 1, right = left + 1;
        if (right<siz && value<(*this)(right,0)) swap = (*this)(left,0)>(*this)(right,0)?left:right;
        else if (left<siz && value<(*this)(left,0)) swap = left;
        else break;
        cimg::swap((*this)(pos,0),(*this)(swap,0));
        cimg::swap((*this)(pos,1),(*this)(swap,1));
        cimg::swap((*this)(pos,2),(*this)(swap,2));
        cimg::swap((*this)(pos,3),(*this)(swap,3));
        pos = swap;
      } while (true);
      return *this;