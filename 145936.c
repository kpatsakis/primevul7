    void JpegBase::readMetadata()
    {
        int rc = 0; // Todo: this should be the return value

        if (io_->open() != 0) throw Error(kerDataSourceOpenFailed, io_->path(), strError());
        IoCloser closer(*io_);
        // Ensure that this is the correct image type
        if (!isThisType(*io_, true)) {
            if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);
            throw Error(kerNotAJpeg);
        }
        clearMetadata();
        int search = 6 ; // Exif, ICC, XMP, Comment, IPTC, SOF
        const long bufMinSize = 36;
        DataBuf buf(bufMinSize);
        Blob psBlob;
        bool foundCompletePsData = false;
        bool foundExifData = false;
        bool foundXmpData = false;
        bool foundIccData = false;

        // Read section marker
        int marker = advanceToMarker();
        if (marker < 0) throw Error(kerNotAJpeg);

        while (marker != sos_ && marker != eoi_ && search > 0) {
            // Read size and signature (ok if this hits EOF)
            std::memset(buf.pData_, 0x0, buf.size_);
            size_t bufRead = io_->read(buf.pData_, bufMinSize);
            if (io_->error())
                throw Error(kerFailedToReadImageData);
            if (bufRead < 2)
                throw Error(kerNotAJpeg);
            uint16_t size = getUShort(buf.pData_, bigEndian);

            if (   !foundExifData
                && marker == app1_ && memcmp(buf.pData_ + 2, exifId_, 6) == 0) {
                if (size < 8) {
                    rc = 1;
                    break;
                }
                // Seek to beginning and read the Exif data
                io_->seek(8 - bufRead, BasicIo::cur);
                DataBuf rawExif(size - 8);
                io_->read(rawExif.pData_, rawExif.size_);
                if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);
                ByteOrder bo = ExifParser::decode(exifData_, rawExif.pData_, (uint32_t)rawExif.size_);
                setByteOrder(bo);
                if (rawExif.size_ > 0 && byteOrder() == invalidByteOrder) {
#ifndef SUPPRESS_WARNINGS
                    EXV_WARNING << "Failed to decode Exif metadata.\n";
#endif
                    exifData_.clear();
                }
                --search;
                foundExifData = true;
            }
            else if (   !foundXmpData
                     && marker == app1_ && memcmp(buf.pData_ + 2, xmpId_, 29) == 0) {
                if (size < 31) {
                    rc = 6;
                    break;
                }
                // Seek to beginning and read the XMP packet
                io_->seek(31 - bufRead, BasicIo::cur);
                DataBuf xmpPacket(size - 31);
                io_->read(xmpPacket.pData_, xmpPacket.size_);
                if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);
                xmpPacket_.assign(reinterpret_cast<char*>(xmpPacket.pData_), xmpPacket.size_);
                if (xmpPacket_.size() > 0 && XmpParser::decode(xmpData_, xmpPacket_)) {
#ifndef SUPPRESS_WARNINGS
                    EXV_WARNING << "Failed to decode XMP metadata.\n";
#endif
                }
                --search;
                foundXmpData = true;
            }
            else if (   !foundCompletePsData
                     && marker == app13_ && memcmp(buf.pData_ + 2, Photoshop::ps3Id_, 14) == 0) {
                if (size < 16) {
                    rc = 2;
                    break;
                }
                // Read the rest of the APP13 segment
                io_->seek(16 - bufRead, BasicIo::cur);
                DataBuf psData(size - 16);
                io_->read(psData.pData_, psData.size_);
                if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);
#ifdef DEBUG
                std::cerr << "Found app13 segment, size = " << size << "\n";
                //hexdump(std::cerr, psData.pData_, psData.size_);
#endif
                // Append to psBlob
                append(psBlob, psData.pData_, (uint32_t)psData.size_);
                // Check whether psBlob is complete
                if (psBlob.size() > 0 && Photoshop::valid(&psBlob[0], (long) psBlob.size())) {
                    --search;
                    foundCompletePsData = true;
                }
            }
            else if (marker == com_ && comment_.empty())
            {
                if (size < 2) {
                    rc = 3;
                    break;
                }
                // JPEGs can have multiple comments, but for now only read
                // the first one (most jpegs only have one anyway). Comments
                // are simple single byte ISO-8859-1 strings.
                io_->seek(2 - bufRead, BasicIo::cur);
                DataBuf comment(size - 2);
                io_->read(comment.pData_, comment.size_);
                if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);
                comment_.assign(reinterpret_cast<char*>(comment.pData_), comment.size_);
                while (   comment_.length()
                       && comment_.at(comment_.length()-1) == '\0') {
                    comment_.erase(comment_.length()-1);
                }
                --search;
            }
            else if ( marker == app2_ && memcmp(buf.pData_ + 2, iccId_,11)==0) {
                if (size < 2+14) {
                    rc = 8;
                    break;
                }
                // ICC profile
                if ( ! foundIccData  ) {
                    foundIccData = true ;
                    --search ;
                }
                int chunk  = (int)    buf.pData_[2+12];
                int chunks = (int)    buf.pData_[2+13];
                // ICC1v43_2010-12.pdf header is 14 bytes
                // header = "ICC_PROFILE\0" (12 bytes)
                // chunk/chunks are a single byte
                // Spec 7.2 Profile bytes 0-3 size
                uint32_t s = getULong(buf.pData_ + (2+14) , bigEndian);
#ifdef DEBUG
                std::cerr << "Found ICC Profile chunk " << chunk
                          << " of "    << chunks
                          << (chunk==1 ? " size: " : "" ) << (chunk==1 ? s : 0)
                          << std::endl  ;
#endif
                io_->seek(-(int64_t)bufRead, BasicIo::cur); // back up to start of buffer (after marker)
                io_->seek(    14+2, BasicIo::cur); // step header
                // read in profile
                // #1286 profile can be padded
                long icc_size = size-2-14;
                if (chunk==1 && chunks==1) {
                  enforce(s <= static_cast<uint32_t>(icc_size), kerInvalidIccProfile);
                  icc_size = s;
                }
                DataBuf icc(icc_size);
                io_->read( icc.pData_,icc.size_);

                if ( !iccProfileDefined() ) { // first block of profile
                    setIccProfile(icc,chunk==chunks);
                } else {                       // extend existing profile
                    DataBuf profile(Safe::add(iccProfile_.size_, icc.size_));
                    if ( iccProfile_.size_ ) {
                        ::memcpy(profile.pData_,iccProfile_.pData_,iccProfile_.size_);
                    }
                    ::memcpy(profile.pData_+iccProfile_.size_,icc.pData_,icc.size_);
                    setIccProfile(profile,chunk==chunks);
                }
            }
            else if (  pixelHeight_ == 0 && inRange2(marker,sof0_,sof3_,sof5_,sof15_) ) {
                // We hit a SOFn (start-of-frame) marker
                if (size < 8) {
                    rc = 7;
                    break;
                }
                pixelHeight_ = getUShort(buf.pData_ + 3, bigEndian);
                pixelWidth_ = getUShort(buf.pData_ + 5, bigEndian);
                if (pixelHeight_ != 0) --search;
                // Skip the remainder of the segment
                io_->seek(size-bufRead, BasicIo::cur);
            }
            else {
                if (size < 2) {
                    rc = 4;
                    break;
                }
                // Skip the remainder of the unknown segment
                if (io_->seek(size - bufRead, BasicIo::cur)) throw Error(kerFailedToReadImageData);
            }
            // Read the beginning of the next segment
            marker = advanceToMarker();
            if (marker < 0) {
                rc = 5;
                break;
            }
        } // while there are segments to process

        if (psBlob.size() > 0) {
            // Find actual IPTC data within the psBlob
            Blob iptcBlob;
            const byte *record = 0;
            uint32_t sizeIptc = 0;
            uint32_t sizeHdr = 0;
            const byte* pCur = &psBlob[0];
            const byte* pEnd = pCur + psBlob.size();
            while (   pCur < pEnd
                   && 0 == Photoshop::locateIptcIrb(pCur, static_cast<long>(pEnd - pCur),
                                                    &record, &sizeHdr, &sizeIptc)) {
#ifdef DEBUG
                std::cerr << "Found IPTC IRB, size = " << sizeIptc << "\n";
#endif
                if (sizeIptc) {
                    append(iptcBlob, record + sizeHdr, sizeIptc);
                }
                pCur = record + sizeHdr + sizeIptc + (sizeIptc & 1);
            }
            if (   iptcBlob.size() > 0
                && IptcParser::decode(iptcData_,
                                      &iptcBlob[0],
                                      static_cast<uint32_t>(iptcBlob.size()))) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Failed to decode IPTC metadata.\n";
#endif
                iptcData_.clear();
            }
        } // psBlob.size() > 0

        if (rc != 0) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "JPEG format error, rc = " << rc << "\n";
#endif
        }
    } // JpegBase::readMetadata