    void RafImage::printStructure(std::ostream& out, PrintStructureOption option, int depth) {
        if (io_->open() != 0) {
            throw Error(kerDataSourceOpenFailed, io_->path(), strError());
        }
        // Ensure this is the correct image type
        if (!isRafType(*io_, true)) {
            if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);
            throw Error(kerNotAnImage, "RAF");
        }
        const bool bPrint = option==kpsBasic || option==kpsRecursive;
        if ( bPrint ) {
            io_->seek(0,BasicIo::beg); // rewind

            {
                out << Internal::indent(depth)
                    << "STRUCTURE OF RAF FILE: "
                    << io().path()
                    << std::endl;
                out << Internal::indent(depth)
                    << Internal::stringFormat("    Length |   Offset | Payload")
                    << std::endl;
            }

            byte magicdata [17];
            io_->read(magicdata, 16);
            magicdata[16] = 0;
            {
                out << Internal::indent(depth)
                    << Internal::stringFormat("  %8u | %8u | ", 16, 0)
                    << "Magic number : "
                    << std::string((char*)&magicdata)
                    << std::endl;
            }

            byte data1 [5];
            io_->read(data1, 4);
            data1[4] = 0;
            {
                out << Internal::indent(depth)
                    << Internal::stringFormat("  %8u | %8u | ", 4, 16)
                    << "data 1 : "
                    << std::string((char*)&data1)
                    << std::endl;
            }

            byte data2 [9];
            io_->read(data2, 8);
            data2[8] = 0;
            {
                out << Internal::indent(depth)
                    << Internal::stringFormat("  %8u | %8u | ", 8, 20)
                    << "data 2 : "
                    << std::string((char*)&data2)
                    << std::endl;
            }

            byte camdata [33];
            io_->read(camdata, 32);
            camdata[32] = 0;
            {
                out << Internal::indent(depth)
                    << Internal::stringFormat("  %8u | %8u | ", 32, 28)
                    << "camera : "
                    << std::string((char*)&camdata)
                    << std::endl;
            }

            byte dir_version [5];
            io_->read(dir_version, 4);
            dir_version[4] = 0;
            {
                out << Internal::indent(depth)
                    << Internal::stringFormat("  %8u | %8u | ", 4, 60)
                    << "dir version : "
                    << std::string((char*)&dir_version)
                    << std::endl;
            }

            byte unknown [20];
            io_->read(unknown, 20);

            byte jpg_img_offset [4];
            io_->read(jpg_img_offset, 4);
            byte jpg_img_length [4];
            io_->read(jpg_img_length, 4);
            long jpg_img_off = Exiv2::getULong((const byte *) jpg_img_offset, bigEndian);
            long jpg_img_len = Exiv2::getULong((const byte *) jpg_img_length, bigEndian);
            std::stringstream j_off;
            std::stringstream j_len;
            j_off << jpg_img_off;
            j_len << jpg_img_len;
            {
                out << Internal::indent(depth)
                    << Internal::stringFormat("  %8u | %8u | ", 4, 84)
                    << "JPEG Image Offset : "
                    << j_off.str()
                    << std::endl;
                out << Internal::indent(depth)
                    << Internal::stringFormat("  %8u | %8u | ", 4, 88)
                    << "JPEG Image Length : "
                    << j_len.str()
                    << std::endl;
            }

            byte cfa_header_offset [4];
            io_->read(cfa_header_offset, 4);
            byte cfa_header_length [4];
            io_->read(cfa_header_length, 4);
            long cfa_hdr_off = Exiv2::getULong((const byte *) cfa_header_offset, bigEndian);
            long cfa_hdr_len = Exiv2::getULong((const byte *) cfa_header_length, bigEndian);
            std::stringstream ch_off;
            std::stringstream ch_len;
            ch_off << cfa_hdr_off;
            ch_len << cfa_hdr_len;
            {
                out << Internal::indent(depth)
                    << Internal::stringFormat("  %8u | %8u | ", 4, 92)
                    << "CFA Header Offset : "
                    << ch_off.str()
                    << std::endl;
                out << Internal::indent(depth)
                    << Internal::stringFormat("  %8u | %8u | ", 4, 96)
                    << "CFA Header Length : "
                    << ch_len.str()
                    << std::endl;
            }

            byte cfa_offset [4];
            io_->read(cfa_offset, 4);
            byte cfa_length [4];
            io_->read(cfa_length, 4);
            long cfa_off = Exiv2::getULong((const byte *) cfa_offset, bigEndian);
            long cfa_len = Exiv2::getULong((const byte *) cfa_length, bigEndian);
            std::stringstream c_off;
            std::stringstream c_len;
            c_off << cfa_off;
            c_len << cfa_len;
            {
                out << Internal::indent(depth)
                    << Internal::stringFormat("  %8u | %8u | ", 4, 100)
                    << "CFA Offset : "
                    << c_off.str()
                    << std::endl;
                out << Internal::indent(depth)
                    << Internal::stringFormat("  %8u | %8u | ", 4, 104)
                    << "CFA Length : "
                    << c_len.str()
                    << std::endl;
            }

            io_->seek(jpg_img_off, BasicIo::beg); // rewind
            DataBuf payload(16); // header is different from chunks
            io_->read(payload.pData_, payload.size_);
            {
                out << Internal::indent(depth)
                    << Internal::stringFormat("  %8u | %8u | ", jpg_img_len, jpg_img_off)
                    << "jpg image / exif : "
                    << Internal::binaryToString(makeSlice(payload, 0, payload.size_))
                    << std::endl;
            }

            io_->seek(cfa_hdr_off, BasicIo::beg); // rewind
            io_->read(payload.pData_, payload.size_);
            {
                out << Internal::indent(depth)
                    << Internal::stringFormat("  %8u | %8u | ", cfa_hdr_len, cfa_hdr_off)
                    << "CFA Header: "
                    << Internal::binaryToString(makeSlice(payload, 0, payload.size_))
                    << std::endl;
            }

            io_->seek(cfa_off, BasicIo::beg); // rewind
            io_->read(payload.pData_, payload.size_);
            {
                out << Internal::indent(depth)
                    << Internal::stringFormat("  %8u | %8u | ", cfa_len, cfa_off)
                    << "CFA : "
                    << Internal::binaryToString(makeSlice(payload, 0, payload.size_))
                    << std::endl;
            }
        }
    } // RafImage::printStructure