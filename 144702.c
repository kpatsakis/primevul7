
    static float _matchpatch(const CImg<T>& img1, const CImg<T>& img2, const CImg<uintT>& occ,
                             const unsigned int psizew, const unsigned int psizeh, const unsigned int psizec,
                             const int x1, const int y1,
                             const int x2, const int y2,
                             const int xc, const int yc,
                             const float occ_penalization,
                             const float max_score) { // 2D version
      const T *p1 = img1.data(x1*psizec,y1), *p2 = img2.data(x2*psizec,y2);
      const unsigned int psizewc = psizew*psizec;
      const ulongT
        offx1 = (ulongT)img1._width - psizewc,
        offx2 = (ulongT)img2._width - psizewc;
      float ssd = 0;
      for (unsigned int j = 0; j<psizeh; ++j) {
        for (unsigned int i = 0; i<psizewc; ++i)
          ssd += cimg::sqr((Tfloat)*(p1++) - *(p2++));
        if (ssd>max_score) return max_score;
        p1+=offx1; p2+=offx2;
      }
      return occ_penalization==0?ssd:cimg::sqr(std::sqrt(ssd) + occ_penalization*occ(xc,yc));