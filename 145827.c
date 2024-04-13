    void XmpSidecar::writeMetadata()
    {
        if (io_->open() != 0) {
            throw Error(kerDataSourceOpenFailed, io_->path(), strError());
        }
        IoCloser closer(*io_);


        if (writeXmpFromPacket() == false) {
            // #589 copy XMP tags
            Exiv2::XmpData  copy   ;
            for (Exiv2::XmpData::const_iterator it = xmpData_.begin(); it != xmpData_.end(); ++it) {
                if ( !matchi(it->key(),"exif") && !matchi(it->key(),"iptc") ) {
                    copy[it->key()] = it->value();
                }
            }

            // run the convertors
            copyExifToXmp(exifData_, xmpData_);
            copyIptcToXmp(iptcData_, xmpData_);

            // #589 - restore tags which were modified by the convertors
            for (Exiv2::XmpData::const_iterator it = copy.begin(); it != copy.end(); ++it) {
                xmpData_[it->key()] = it->value() ;
            }

            // #1112 - restore dates if they lost their TZ info
            for ( Exiv2::Dictionary_i it = dates_.begin() ; it != dates_.end() ; ++it ) {
                std::string   sKey = it->first;
                Exiv2::XmpKey key(sKey);
                if ( xmpData_.findKey(key) != xmpData_.end() ) {
                    std::string value_orig(it->second);
                    std::string value_now(xmpData_[sKey].value().toString());
                    // std::cout << key << " -> " << value_now << " => " << value_orig << std::endl;
                    if ( value_orig.find(value_now.substr(0,10)) != std::string::npos ) {
                        xmpData_[sKey] = value_orig ;
                    }
                }
            }

            if (XmpParser::encode(xmpPacket_, xmpData_,
                                  XmpParser::omitPacketWrapper|XmpParser::useCompactFormat) > 1) {
#ifndef SUPPRESS_WARNINGS
                EXV_ERROR << "Failed to encode XMP metadata.\n";
#endif
            }
        }
        if (xmpPacket_.size() > 0) {
            if (xmpPacket_.substr(0, 5)  != "<?xml") {
                xmpPacket_ = xmlHeader + xmpPacket_ + xmlFooter;
            }
            BasicIo::UniquePtr tempIo(new MemIo);
            assert(tempIo.get() != 0);
            // Write XMP packet
            if (tempIo->write(reinterpret_cast<const byte*>(xmpPacket_.data()), xmpPacket_.size()) != xmpPacket_.size())
                throw Error(kerImageWriteFailed);
            if (tempIo->error()) throw Error(kerImageWriteFailed);
            io_->close();
            io_->transfer(*tempIo); // may throw
        }
    } // XmpSidecar::writeMetadata