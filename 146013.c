    static uint32_t byteSwap_(Exiv2::DataBuf& buf,size_t offset,bool bSwap)
    {
        uint32_t v;
        char*    p = (char*) &v;
        int      i;
        for ( i = 0 ; i < 4 ; i++ ) p[i] = buf.pData_[offset+i];
        uint32_t result = byteSwap_(v,bSwap);
        p               = (char*) &result;
        for ( i = 0 ; i < 4 ; i++ ) buf.pData_[offset+i] = p[i];
        return result;
    }