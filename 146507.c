    TiffReader::TiffReader(const byte*    pData,
                           uint32_t       size,
                           TiffComponent* pRoot,
                           TiffRwState    state)
        : pData_(pData),
          size_(size),
          pLast_(pData + size),
          pRoot_(pRoot),
          origState_(state),
          mnState_(state),
          postProc_(false)
    {
        pState_ = &origState_;
        assert(pData_);
        assert(size_ > 0);

    } // TiffReader::TiffReader