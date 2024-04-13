    DataBuf TiffHeaderBase::write() const
    {
        DataBuf buf(8);
        switch (byteOrder_) {
        case littleEndian:
            buf.pData_[0] = 'I';
            break;
        case bigEndian:
            buf.pData_[0] = 'M';
            break;
        case invalidByteOrder:
            assert(false);
            break;
        }
        buf.pData_[1]=buf.pData_[0];
        us2Data(buf.pData_ + 2, tag_, byteOrder_);
        ul2Data(buf.pData_ + 4, 0x00000008, byteOrder_);
        return buf;
    }