    void Jp2Image::readMetadata()
    {
#ifdef DEBUG
        std::cerr << "Exiv2::Jp2Image::readMetadata: Reading JPEG-2000 file " << io_->path() << std::endl;
#endif
        if (io_->open() != 0)
        {
            throw Error(kerDataSourceOpenFailed, io_->path(), strError());
        }
        IoCloser closer(*io_);
        // Ensure that this is the correct image type
        if (!isJp2Type(*io_, true))
        {
            if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);
            throw Error(kerNotAnImage, "JPEG-2000");
        }

        Jp2BoxHeader      box       = {0,0};
        Jp2BoxHeader      subBox    = {0,0};
        Jp2ImageHeaderBox ihdr      = {0,0,0,0,0,0,0,0};
        Jp2UuidBox        uuid      = {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

        while (io_->read((byte*)&box, sizeof(box)) == sizeof(box))
        {
            long position = io_->tell();
            box.length = getLong((byte*)&box.length, bigEndian);
            box.type   = getLong((byte*)&box.type, bigEndian);
#ifdef DEBUG
            std::cout << "Exiv2::Jp2Image::readMetadata: "
                      << "Position: " << position
                      << " box type: " << toAscii(box.type)
                      << " length: " << box.length
                      << std::endl;
#endif

            if (box.length == 0) return ;

            if (box.length == 1)
            {
                // FIXME. Special case. the real box size is given in another place.
            }

            switch(box.type)
            {
                case kJp2BoxTypeJp2Header:
                {
#ifdef DEBUG
                    std::cout << "Exiv2::Jp2Image::readMetadata: JP2Header box found" << std::endl;
#endif
                    long restore = io_->tell();

                    while (io_->read((byte*)&subBox, sizeof(subBox)) == sizeof(subBox) && subBox.length )
                    {
                        subBox.length = getLong((byte*)&subBox.length, bigEndian);
                        subBox.type   = getLong((byte*)&subBox.type, bigEndian);
#ifdef DEBUG
                        std::cout << "Exiv2::Jp2Image::readMetadata: "
                        << "subBox = " << toAscii(subBox.type) << " length = " << subBox.length << std::endl;
#endif
                        if(subBox.type == kJp2BoxTypeColorHeader && subBox.length != 15)
                        {
#ifdef DEBUG
                            std::cout << "Exiv2::Jp2Image::readMetadata: "
                                     << "Color data found" << std::endl;
#endif

                            const long pad = 3 ; // 3 padding bytes 2 0 0
                            const size_t data_length = Safe::add(subBox.length, static_cast<uint32_t>(8));
                            // data_length makes no sense if it is larger than the rest of the file
                            if (data_length > io_->size() - io_->tell()) {
                                throw Error(kerCorruptedMetadata);
                            }
                            DataBuf data(static_cast<long>(data_length));
                            io_->read(data.pData_,data.size_);
                            const size_t iccLength = getULong(data.pData_+pad, bigEndian);
                            // subtracting pad from data.size_ is safe:
                            // size_ is at least 8 and pad = 3
                            if (iccLength > data.size_ - pad) {
                                throw Error(kerCorruptedMetadata);
                            }
                            DataBuf icc(iccLength);
                            ::memcpy(icc.pData_,data.pData_+pad,icc.size_);
#ifdef DEBUG
                            const char* iccPath = "/tmp/libexiv2_jp2.icc";
                            FILE* f = fopen(iccPath,"wb");
                            if ( f ) {
                                fwrite(icc.pData_,icc.size_,1,f);
                                fclose(f);
                            }
                            std::cout << "Exiv2::Jp2Image::readMetadata: wrote iccProfile " << icc.size_<< " bytes to " << iccPath << std::endl ;
#endif
                            setIccProfile(icc);
                        }

                        if( subBox.type == kJp2BoxTypeImageHeader)
                        {
                            io_->read((byte*)&ihdr, sizeof(ihdr));
#ifdef DEBUG
                            std::cout << "Exiv2::Jp2Image::readMetadata: Ihdr data found" << std::endl;
#endif
                            ihdr.imageHeight            = getLong((byte*)&ihdr.imageHeight, bigEndian);
                            ihdr.imageWidth             = getLong((byte*)&ihdr.imageWidth, bigEndian);
                            ihdr.componentCount         = getShort((byte*)&ihdr.componentCount, bigEndian);
                            ihdr.compressionTypeProfile = getShort((byte*)&ihdr.compressionTypeProfile, bigEndian);

                            pixelWidth_  = ihdr.imageWidth;
                            pixelHeight_ = ihdr.imageHeight;
                        }

                        io_->seek(restore,BasicIo::beg);
                        io_->seek(subBox.length, Exiv2::BasicIo::cur);
                        restore = io_->tell();
                    }
                    break;
                }

                case kJp2BoxTypeUuid:
                {
#ifdef DEBUG
                    std::cout << "Exiv2::Jp2Image::readMetadata: UUID box found" << std::endl;
#endif

                    if (io_->read((byte*)&uuid, sizeof(uuid)) == sizeof(uuid))
                    {
                        DataBuf rawData;
                        size_t  bufRead;
                        bool    bIsExif = memcmp(uuid.uuid, kJp2UuidExif, sizeof(uuid))==0;
                        bool    bIsIPTC = memcmp(uuid.uuid, kJp2UuidIptc, sizeof(uuid))==0;
                        bool    bIsXMP  = memcmp(uuid.uuid, kJp2UuidXmp , sizeof(uuid))==0;

                        if(bIsExif)
                        {
#ifdef DEBUG
                           std::cout << "Exiv2::Jp2Image::readMetadata: Exif data found" << std::endl ;
#endif
                            rawData.alloc(box.length - (sizeof(box) + sizeof(uuid)));
                            bufRead = io_->read(rawData.pData_, rawData.size_);
                            if (io_->error()) throw Error(kerFailedToReadImageData);
                            if (bufRead != rawData.size_) throw Error(kerInputDataReadFailed);

                            if (rawData.size_ > 1)
                            {
                                bool foundPos{ false };
                                size_t pos{ 0 };
                                if ((rawData.pData_[0] == rawData.pData_[1]) &&
                                    (rawData.pData_[0] == 'I' || rawData.pData_[0] == 'M')) {
                                    foundPos = true;
                                } else {
                                    const std::array<byte, 6> exifHeader{ 0x45, 0x78, 0x69, 0x66, 0x00, 0x00 };
                                    const auto& it = std::search(rawData.cbegin(), rawData.cend(), exifHeader.cbegin(), exifHeader.cend());
                                    if (it != rawData.cend()) {
                                        pos = it - rawData.cbegin() + exifHeader.size();
                                        foundPos = true;
#ifndef SUPPRESS_WARNINGS
                                        EXV_WARNING << "Reading non-standard UUID-EXIF_bad box in " << io_->path() << std::endl;
#endif

                                    }
                                }

                                // If found it, store only these data at from this place.
                                if (foundPos)
                                {
#ifdef DEBUG
                                    std::cout << "Exiv2::Jp2Image::readMetadata: Exif header found at position " << pos << std::endl;
#endif
                                    ByteOrder bo = TiffParser::decode(exifData(),
                                                                      iptcData(),
                                                                      xmpData(),
                                                                      rawData.pData_ + pos,
                                                                      (uint32_t)(rawData.size_ - pos));
                                    setByteOrder(bo);
                                }
                            }
                            else
                            {
#ifndef SUPPRESS_WARNINGS
                                EXV_WARNING << "Failed to decode Exif metadata." << std::endl;
#endif
                                exifData_.clear();
                            }
                        }

                        if(bIsIPTC)
                        {
#ifdef DEBUG
                           std::cout << "Exiv2::Jp2Image::readMetadata: Iptc data found" << std::endl;
#endif
                            rawData.alloc(box.length - (sizeof(box) + sizeof(uuid)));
                            bufRead = io_->read(rawData.pData_, rawData.size_);
                            if (io_->error()) throw Error(kerFailedToReadImageData);
                            if (bufRead != rawData.size_) throw Error(kerInputDataReadFailed);

                            if (IptcParser::decode(iptcData_, rawData.pData_, rawData.size_))
                            {
#ifndef SUPPRESS_WARNINGS
                                EXV_WARNING << "Failed to decode IPTC metadata." << std::endl;
#endif
                                iptcData_.clear();
                            }
                        }

                        if(bIsXMP)
                        {
#ifdef DEBUG
                           std::cout << "Exiv2::Jp2Image::readMetadata: Xmp data found" << std::endl;
#endif
                            rawData.alloc(box.length - (uint32_t)(sizeof(box) + sizeof(uuid)));
                            bufRead = io_->read(rawData.pData_, rawData.size_);
                            if (io_->error()) throw Error(kerFailedToReadImageData);
                            if (bufRead != rawData.size_) throw Error(kerInputDataReadFailed);
                            xmpPacket_.assign(reinterpret_cast<char *>(rawData.pData_), rawData.size_);

                            std::string::size_type idx = xmpPacket_.find_first_of('<');
                            if (idx != std::string::npos && idx > 0)
                            {
#ifndef SUPPRESS_WARNINGS
                                EXV_WARNING << "Removing " << static_cast<uint32_t>(idx)
                                            << " characters from the beginning of the XMP packet" << std::endl;
#endif
                                xmpPacket_ = xmpPacket_.substr(idx);
                            }

                            if (xmpPacket_.size() > 0 && XmpParser::decode(xmpData_, xmpPacket_))
                            {
#ifndef SUPPRESS_WARNINGS
                                EXV_WARNING << "Failed to decode XMP metadata." << std::endl;
#endif
                            }
                        }
                    }
                    break;
                }

                default:
                {
                    break;
                }
            }

            // Move to the next box.
            io_->seek(static_cast<long>(position - sizeof(box) + box.length), BasicIo::beg);
            if (io_->error()) throw Error(kerFailedToReadImageData);
        }

    } // Jp2Image::readMetadata