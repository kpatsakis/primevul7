    template<typename t>
    double dot(const CImg<t>& img) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "dot(): Empty instance.",
                                    cimg_instance);
      if (!img)
        throw CImgArgumentException(_cimg_instance
                                    "dot(): Empty specified image.",
                                    cimg_instance);

      const ulongT nb = std::min(size(),img.size());
      double res = 0;
      for (ulongT off = 0; off<nb; ++off) res+=(double)_data[off]*(double)img[off];
      return res;