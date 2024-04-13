    void TiffReader::readTiffEntry(TiffEntryBase* object)
    {
        assert(object != 0);

        byte* p = object->start();
        assert(p >= pData_);

        if (p + 12 > pLast_) {
#ifndef SUPPRESS_WARNINGS
            EXV_ERROR << "Entry in directory " << groupName(object->group())
                      << "requests access to memory beyond the data buffer. "
                      << "Skipping entry.\n";
#endif
            return;
        }
        // Component already has tag
        p += 2;
        TiffType tiffType = getUShort(p, byteOrder());
        TypeId typeId = toTypeId(tiffType, object->tag(), object->group());
        long typeSize = TypeInfo::typeSize(typeId);
        if (0 == typeSize) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Directory " << groupName(object->group())
                        << ", entry 0x" << std::setw(4)
                        << std::setfill('0') << std::hex << object->tag()
                        << " has unknown Exif (TIFF) type " << std::dec << tiffType
                        << "; setting type size 1.\n";
#endif
            typeSize = 1;
        }
        p += 2;
        uint32_t count = getULong(p, byteOrder());
        if (count >= 0x10000000) {
#ifndef SUPPRESS_WARNINGS
            EXV_ERROR << "Directory " << groupName(object->group())
                      << ", entry 0x" << std::setw(4)
                      << std::setfill('0') << std::hex << object->tag()
                      << " has invalid size "
                      << std::dec << count << "*" << typeSize
                      << "; skipping entry.\n";
#endif
            return;
        }
        p += 4;
        uint32_t isize= 0; // size of Exif.Sony1.PreviewImage

        if (count > std::numeric_limits<uint32_t>::max() / typeSize) {
            throw Error(kerArithmeticOverflow);
        }
        uint32_t size = typeSize * count;
        uint32_t offset = getLong(p, byteOrder());
        byte* pData = p;
        if (   size > 4
            && (   baseOffset() + offset >= size_
                || static_cast<int32_t>(baseOffset()) + offset <= 0)) {
                // #1143
                if ( object->tag() == 0x2001 && std::string(groupName(object->group())) == "Sony1" ) {
                    isize=size;
                } else {
#ifndef SUPPRESS_WARNINGS
            EXV_ERROR << "Offset of directory " << groupName(object->group())
                      << ", entry 0x" << std::setw(4)
                      << std::setfill('0') << std::hex << object->tag()
                      << " is out of bounds: "
                      << "Offset = 0x" << std::setw(8)
                      << std::setfill('0') << std::hex << offset
                      << "; truncating the entry\n";
#endif
                }
                size = 0;
        }
        if (size > 4) {
            // setting pData to pData_ + baseOffset() + offset can result in pData pointing to invalid memory,
            // as offset can be arbitrarily large
            if ((static_cast<uintptr_t>(baseOffset()) > std::numeric_limits<uintptr_t>::max() - static_cast<uintptr_t>(offset))
             || (static_cast<uintptr_t>(baseOffset() + offset) > std::numeric_limits<uintptr_t>::max() - reinterpret_cast<uintptr_t>(pData_)))
            {
                throw Error(kerCorruptedMetadata); // #562 don't throw kerArithmeticOverflow
            }
            if (pData_ + static_cast<uintptr_t>(baseOffset()) + static_cast<uintptr_t>(offset) > pLast_) {
                throw Error(kerCorruptedMetadata);
            }
            pData = const_cast<byte*>(pData_) + baseOffset() + offset;

        // check for size being invalid
            if (size > static_cast<uint32_t>(pLast_ - pData)) {
#ifndef SUPPRESS_WARNINGS
                EXV_ERROR << "Upper boundary of data for "
                          << "directory " << groupName(object->group())
                          << ", entry 0x" << std::setw(4)
                          << std::setfill('0') << std::hex << object->tag()
                          << " is out of bounds: "
                          << "Offset = 0x" << std::setw(8)
                          << std::setfill('0') << std::hex << offset
                          << ", size = " << std::dec << size
                          << ", exceeds buffer size by "
                          // cast to make MSVC happy
                          << static_cast<uint32_t>(pData + size - pLast_)
                          << " Bytes; truncating the entry\n";
#endif
                size = 0;
            }
        }
        Value::UniquePtr v = Value::create(typeId);
        enforce(v.get() != nullptr, kerCorruptedMetadata);
        if ( !isize ) {
            v->read(pData, size, byteOrder());
        } else {
            // #1143 Write a "hollow" buffer for the preview image
            //       Sadly: we don't know the exact location of the image in the source (it's near offset)
            //       And neither TiffReader nor TiffEntryBase have access to the BasicIo object being processed
            byte* buffer = (byte*) ::malloc(isize);
            ::memset(buffer,0,isize);
            v->read(buffer,isize, byteOrder());
            ::free(buffer);
        }

        object->setValue(std::move(v));
        object->setData(pData, size);
        object->setOffset(offset);
        object->setIdx(nextIdx(object->group()));

    } // TiffReader::readTiffEntry