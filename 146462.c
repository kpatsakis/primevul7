    void TiffBinaryArray::iniOrigDataBuf()
    {
        origData_ = const_cast<byte*>(pData());
        origSize_ = TiffEntryBase::doSize();
    }