    void JpegBase::printStructure(std::ostream& out, PrintStructureOption option, int depth)
    {
        if (io_->open() != 0)
            throw Error(kerDataSourceOpenFailed, io_->path(), strError());
        // Ensure that this is the correct image type
        if (!isThisType(*io_, false)) {
            if (io_->error() || io_->eof())
                throw Error(kerFailedToReadImageData);
            throw Error(kerNotAJpeg);
        }

        bool bPrint = option == kpsBasic || option == kpsRecursive;
        Exiv2::Uint32Vector iptcDataSegs;

        if (bPrint || option == kpsXMP || option == kpsIccProfile || option == kpsIptcErase) {
            // nmonic for markers
            std::string nm[256];
            nm[0xd8] = "SOI";
            nm[0xd9] = "EOI";
            nm[0xda] = "SOS";
            nm[0xdb] = "DQT";
            nm[0xdd] = "DRI";
            nm[0xfe] = "COM";

            // 0xe0 .. 0xef are APPn
            // 0xc0 .. 0xcf are SOFn (except 4)
            nm[0xc4] = "DHT";
            for (int i = 0; i <= 15; i++) {
                char MN[10];
                sprintf(MN, "APP%d", i);
                nm[0xe0 + i] = MN;
                if (i != 4) {
                    sprintf(MN, "SOF%d", i);
                    nm[0xc0 + i] = MN;
                }
            }

            // which markers have a length field?
            bool mHasLength[256];
            for (int i = 0; i < 256; i++)
                mHasLength[i] = (i >= sof0_ && i <= sof15_) || (i >= app0_ && i <= (app0_ | 0x0F)) ||
                                (i == dht_ || i == dqt_ || i == dri_ || i == com_ || i == sos_);

            // Container for the signature
            bool bExtXMP = false;
            const long bufMinSize = 36;
            DataBuf buf(bufMinSize);

            // Read section marker
            int marker = advanceToMarker();
            if (marker < 0)
                throw Error(kerNotAJpeg);

            bool done = false;
            bool first = true;
            while (!done) {
                // print marker bytes
                if (first && bPrint) {
                    out << "STRUCTURE OF JPEG FILE: " << io_->path() << std::endl;
                    out << " address | marker       |  length | data" << std::endl;
                    REPORT_MARKER;
                }
                first = false;
                bool bLF = bPrint;

                // Read size and signature
                std::memset(buf.pData_, 0x0, buf.size_);
                size_t bufRead = io_->read(buf.pData_, bufMinSize);
                if (io_->error())
                    throw Error(kerFailedToReadImageData);
                if (bufRead < 2)
                    throw Error(kerNotAJpeg);
                const uint16_t size = mHasLength[marker] ? getUShort(buf.pData_, bigEndian) : 0;
                if (bPrint && mHasLength[marker])
                    out << Internal::stringFormat(" | %7d ", size);

                // print signature for APPn
                if (marker >= app0_ && marker <= (app0_ | 0x0F)) {
                    // http://www.adobe.com/content/dam/Adobe/en/devnet/xmp/pdfs/XMPSpecificationPart3.pdf p75
                    const std::string signature =
                        string_from_unterminated(reinterpret_cast<const char*>(buf.pData_ + 2), buf.size_ - 2);

                    // 728 rmills@rmillsmbp:~/gnu/exiv2/ttt $ exiv2 -pS test/data/exiv2-bug922.jpg
                    // STRUCTURE OF JPEG FILE: test/data/exiv2-bug922.jpg
                    // address | marker     | length  | data
                    //       0 | 0xd8 SOI   |       0
                    //       2 | 0xe1 APP1  |     911 | Exif..MM.*.......%.........#....
                    //     915 | 0xe1 APP1  |     870 | http://ns.adobe.com/xap/1.0/.<x:
                    //    1787 | 0xe1 APP1  |   65460 | http://ns.adobe.com/xmp/extensio
                    if (option == kpsXMP && signature.find("http://ns.adobe.com/x") == 0) {
                        // extract XMP
                        if (size > 0) {
                            io_->seek(-(int64_t)bufRead, BasicIo::cur);
                            std::vector<byte> xmp(size + 1);
                            io_->read(&xmp[0], size);
                            int start = 0;

                            // http://wwwimages.adobe.com/content/dam/Adobe/en/devnet/xmp/pdfs/XMPSpecificationPart3.pdf
                            // if we find HasExtendedXMP, set the flag and ignore this block
                            // the first extended block is a copy of the Standard block.
                            // a robust implementation allows extended blocks to be out of sequence
                            // we could implement out of sequence with a dictionary of sequence/offset
                            // and dumping the XMP in a post read operation similar to kpsIptcErase
                            // for the moment, dumping 'on the fly' is working fine
                            if (!bExtXMP) {
                                while (xmp.at(start)) {
                                    start++;
                                }
                                start++;
                                const std::string xmp_from_start = string_from_unterminated(
                                    reinterpret_cast<const char*>(&xmp.at(start)), size - start);
                                if (xmp_from_start.find("HasExtendedXMP", start) != xmp_from_start.npos) {
                                    start = size;  // ignore this packet, we'll get on the next time around
                                    bExtXMP = true;
                                }
                            } else {
                                start = 2 + 35 + 32 + 4 + 4;  // Adobe Spec, p19
                            }

                            out.write(reinterpret_cast<const char*>(&xmp.at(start)), size - start);
                            bufRead = size;
                            done = !bExtXMP;
                        }
                    } else if (option == kpsIccProfile && signature.compare(iccId_) == 0) {
                        // extract ICCProfile
                        if (size > 0) {
                            io_->seek(-(int64_t)bufRead, BasicIo::cur);  // back to buffer (after marker)
                            io_->seek(14 + 2, BasicIo::cur);    // step over header
                            DataBuf icc(size - (14 + 2));
                            io_->read(icc.pData_, icc.size_);
                            out.write(reinterpret_cast<const char*>(icc.pData_), icc.size_);
#ifdef DEBUG
                            std::cout << "iccProfile size = " << icc.size_ << std::endl;
#endif
                            bufRead = size;
                        }
                    } else if (option == kpsIptcErase && signature.compare("Photoshop 3.0") == 0) {
                        // delete IPTC data segment from JPEG
                        if (size > 0) {
                            io_->seek(-(int64_t)bufRead, BasicIo::cur);
                            iptcDataSegs.push_back(io_->tell());
                            iptcDataSegs.push_back(size);
                        }
                    } else if (bPrint) {
                        const size_t start = size > 0 ? 2 : 0;
                        const size_t end = start + (size > 32 ? 32 : size);
                        out << "| " << Internal::binaryToString(makeSlice(buf, start, end));
                        if (signature.compare(iccId_) == 0) {
                            // extract the chunk information from the buffer
                            //
                            // the buffer looks like this in this branch
                            // ICC_PROFILE\0AB
                            // where A & B are bytes (the variables chunk & chunks)
                            //
                            // We cannot extract the variables A and B from the signature string, as they are beyond the
                            // null termination (and signature ends there).
                            // => Read the chunk info from the DataBuf directly
                            enforce<std::out_of_range>(buf.size_ - 2 > 14, "Buffer too small to extract chunk information.");
                            const int chunk = buf.pData_[2 + 12];
                            const int chunks = buf.pData_[2 + 13];
                            out << Internal::stringFormat(" chunk %d/%d", chunk, chunks);
                        }
                    }

                    // for MPF: http://www.sno.phy.queensu.ca/~phil/exiftool/TagNames/MPF.html
                    // for FLIR: http://owl.phy.queensu.ca/~phil/exiftool/TagNames/FLIR.html
                    bool bFlir = option == kpsRecursive && marker == (app0_ + 1) && signature.compare("FLIR") == 0;
                    bool bExif = option == kpsRecursive && marker == (app0_ + 1) && signature.compare("Exif") == 0;
                    bool bMPF = option == kpsRecursive && marker == (app0_ + 2) && signature.compare("MPF") == 0;
                    bool bPS = option == kpsRecursive && signature.compare("Photoshop 3.0") == 0;
                    if (bFlir || bExif || bMPF || bPS) {
                        // extract Exif data block which is tiff formatted
                        if (size > 0) {
                            out << std::endl;

                            // allocate storage and current file position
                            byte* exif = new byte[size];
                            uint32_t restore = io_->tell();

                            // copy the data to memory
                            io_->seek(-(int64_t)bufRead, BasicIo::cur);
                            io_->read(exif, size);
                            uint32_t start = signature.compare("Exif") == 0 ? 8 : 6;
                            uint32_t max = (uint32_t)size - 1;

                            // is this an fff block?
                            if (bFlir) {
                                start = 0;
                                bFlir = false;
                                while (start < max) {
                                    if (std::strcmp((const char*)(exif + start), "FFF") == 0) {
                                        bFlir = true;
                                        break;
                                    }
                                    start++;
                                }
                            }

                            // there is a header in FLIR, followed by a tiff block
                            // Hunt down the tiff using brute force
                            if (bFlir) {
                                // FLIRFILEHEAD* pFFF = (FLIRFILEHEAD*) (exif+start) ;
                                while (start < max) {
                                    if (exif[start] == 'I' && exif[start + 1] == 'I')
                                        break;
                                    if (exif[start] == 'M' && exif[start + 1] == 'M')
                                        break;
                                    start++;
                                }
                                if (start < max)
                                    std::cout << "  FFF start = " << start << std::endl;
                                // << " index = " << pFFF->dwIndexOff << std::endl;
                            }

                            if (bPS) {
                                IptcData::printStructure(out, makeSlice(exif, 0, size), depth);
                            } else {
                                // create a copy on write memio object with the data, then print the structure
                                BasicIo::UniquePtr p = BasicIo::UniquePtr(new MemIo(exif + start, size - start));
                                if (start < max)
                                    printTiffStructure(*p, out, option, depth);
                            }

                            // restore and clean up
                            io_->seek(restore, Exiv2::BasicIo::beg);
                            delete[] exif;
                            bLF = false;
                        }
                    }
                }

                // print COM marker
                if (bPrint && marker == com_) {
                    // size includes 2 for the two bytes for size!
                    const int n = (size - 2) > 32 ? 32 : size - 2;
                    // start after the two bytes
                    out << "| "
                        << Internal::binaryToString(
                               makeSlice(buf, 2, n + 2 /* cannot overflow as n is at most size - 2 */));
                }

                // Skip the segment if the size is known
                if (io_->seek(size - bufRead, BasicIo::cur))
                    throw Error(kerFailedToReadImageData);

                if (bLF)
                    out << std::endl;

                if (marker != sos_) {
                    // Read the beginning of the next segment
                    marker = advanceToMarker();
                    enforce(marker>=0, kerNoImageInInputData);
                    REPORT_MARKER;
                }
                done |= marker == eoi_ || marker == sos_;
                if (done && bPrint)
                    out << std::endl;
            }
        }
        if (option == kpsIptcErase && iptcDataSegs.size()) {
#ifdef DEBUG
            std::cout << "iptc data blocks: " << iptcDataSegs.size() << std::endl;
            uint32_t toggle = 0;
            for (Uint32Vector_i i = iptcDataSegs.begin(); i != iptcDataSegs.end(); i++) {
                std::cout << *i;
                if (toggle++ % 2)
                    std::cout << std::endl;
                else
                    std::cout << ' ';
            }
#endif
            uint32_t count = (uint32_t)iptcDataSegs.size();

            // figure out which blocks to copy
            uint64_t* pos = new uint64_t[count + 2];
            pos[0] = 0;
            // copy the data that is not iptc
            Uint32Vector_i it = iptcDataSegs.begin();
            for (uint64_t i = 0; i < count; i++) {
                bool bOdd = (i % 2) != 0;
                bool bEven = !bOdd;
                pos[i + 1] = bEven ? *it : pos[i] + *it;
                ++it;
            }
            pos[count + 1] = io_->size() - pos[count];
#ifdef DEBUG
            for (uint64_t i = 0; i < count + 2; i++)
                std::cout << pos[i] << " ";
            std::cout << std::endl;
#endif
            // $ dd bs=1 skip=$((0)) count=$((13164)) if=ETH0138028.jpg of=E1.jpg
            // $ dd bs=1 skip=$((49304)) count=2000000  if=ETH0138028.jpg of=E2.jpg
            // cat E1.jpg E2.jpg > E.jpg
            // exiv2 -pS E.jpg

            // binary copy io_ to a temporary file
            BasicIo::UniquePtr tempIo(new MemIo);

            assert(tempIo.get() != 0);
            for (uint64_t i = 0; i < (count / 2) + 1; i++) {
                uint64_t start = pos[2 * i] + 2;  // step JPG 2 byte marker
                if (start == 2)
                    start = 0;  // read the file 2 byte SOI
                long length = (long)(pos[2 * i + 1] - start);
                if (length) {
#ifdef DEBUG
                    std::cout << start << ":" << length << std::endl;
#endif
                    io_->seek(start, BasicIo::beg);
                    DataBuf buf(length);
                    io_->read(buf.pData_, buf.size_);
                    tempIo->write(buf.pData_, buf.size_);
                }
            }
            delete[] pos;

            io_->seek(0, BasicIo::beg);
            io_->transfer(*tempIo);  // may throw
            io_->seek(0, BasicIo::beg);
            readMetadata();
        }
    }  // JpegBase::printStructure