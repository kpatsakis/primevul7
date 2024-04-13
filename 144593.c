    template<typename t>
    bool is_overlapped(const CImg<t>& img) const {
      const ulongT csiz = size(), isiz = img.size();
      return !((void*)(_data + csiz)<=(void*)img._data || (void*)_data>=(void*)(img._data + isiz));