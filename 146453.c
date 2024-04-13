    void Jp2Image::encodeJp2Header(const DataBuf& boxBuf,DataBuf& outBuf)
    {
        DataBuf output(boxBuf.size_ + iccProfile_.size_ + 100); // allocate sufficient space
        int     outlen = sizeof(Jp2BoxHeader) ; // now many bytes have we written to output?
        int      inlen = sizeof(Jp2BoxHeader) ; // how many bytes have we read from boxBuf?
        Jp2BoxHeader* pBox   = (Jp2BoxHeader*) boxBuf.pData_;
        int32_t       length = getLong((byte*)&pBox->length, bigEndian);
        int32_t       count  = sizeof (Jp2BoxHeader);
        char*         p      = (char*) boxBuf.pData_;
        bool          bWroteColor = false ;

        while ( count < length || !bWroteColor ) {
            Jp2BoxHeader* pSubBox = (Jp2BoxHeader*) (p+count) ;

            // copy data.  pointer could be into a memory mapped file which we will decode!
            Jp2BoxHeader   subBox = *pSubBox ;
            Jp2BoxHeader   newBox =  subBox;

            if ( count < length ) {
                subBox.length = getLong((byte*)&subBox.length, bigEndian);
                subBox.type   = getLong((byte*)&subBox.type  , bigEndian);
#ifdef DEBUG
                std::cout << "Jp2Image::encodeJp2Header subbox: "<< toAscii(subBox.type) << " length = " << subBox.length << std::endl;
#endif
                count        += subBox.length;
                newBox.type   = subBox.type;
            } else {
                subBox.length=0;
                newBox.type = kJp2BoxTypeColorHeader;
                count = length;
            }

            int32_t newlen = subBox.length;
            if ( newBox.type == kJp2BoxTypeColorHeader ) {
                bWroteColor = true ;
                if ( ! iccProfileDefined() ) {
                    const char* pad   = "\x01\x00\x00\x00\x00\x00\x10\x00\x00\x05\x1cuuid";
                    uint32_t    psize = 15;
                    ul2Data((byte*)&newBox.length,psize      ,bigEndian);
                    ul2Data((byte*)&newBox.type  ,newBox.type,bigEndian);
                    ::memcpy(output.pData_+outlen                     ,&newBox            ,sizeof(newBox));
                    ::memcpy(output.pData_+outlen+sizeof(newBox)      ,pad                ,psize         );
                    newlen = psize ;
                } else {
                    const char* pad   = "\0x02\x00\x00";
                    uint32_t    psize = 3;
                    ul2Data((byte*)&newBox.length, psize + static_cast<uint32_t>(iccProfile_.size_), bigEndian);
                    ul2Data((byte*)&newBox.type, newBox.type, bigEndian);
                    ::memcpy(output.pData_+outlen                     ,&newBox            ,sizeof(newBox)  );
                    ::memcpy(output.pData_+outlen+sizeof(newBox)      , pad               ,psize           );
                    ::memcpy(output.pData_+outlen+sizeof(newBox)+psize,iccProfile_.pData_,iccProfile_.size_);
                    newlen = psize + static_cast<uint32_t>(iccProfile_.size_);
                }
            } else {
                ::memcpy(output.pData_+outlen,boxBuf.pData_+inlen,subBox.length);
            }

            outlen += newlen;
            inlen  += subBox.length;
        }

        // allocate the correct number of bytes, copy the data and update the box header
        outBuf.alloc(outlen);
        ::memcpy(outBuf.pData_,output.pData_,outlen);
        pBox   = (Jp2BoxHeader*) outBuf.pData_;
        ul2Data((byte*)&pBox->type,kJp2BoxTypeJp2Header,bigEndian);
        ul2Data((byte*)&pBox->length,outlen,bigEndian);
    } // Jp2Image::encodeJp2Header