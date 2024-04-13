    void TiffReader::visitSubIfd(TiffSubIfd* object)
    {
        assert(object != 0);

        readTiffEntry(object);
        if (   (object->tiffType() == ttUnsignedLong || object->tiffType() == ttSignedLong
                || object->tiffType() == ttTiffIfd)
            && object->count() >= 1) {
            // Todo: Fix hack
            uint32_t maxi = 9;
            if (object->group() == ifd1Id) maxi = 1;
            for (uint32_t i = 0; i < object->count(); ++i) {
                uint32_t offset = getLong(object->pData() + 4*i, byteOrder());
                if (   baseOffset() + offset > size_ ) {
#ifndef SUPPRESS_WARNINGS
                    EXV_ERROR << "Directory " << groupName(object->group())
                              << ", entry 0x" << std::setw(4)
                              << std::setfill('0') << std::hex << object->tag()
                              << " Sub-IFD pointer " << i
                              << " is out of bounds; ignoring it.\n";
#endif
                    return;
                }
                if (i >= maxi) {
#ifndef SUPPRESS_WARNINGS
                    EXV_WARNING << "Directory " << groupName(object->group())
                                << ", entry 0x" << std::setw(4)
                                << std::setfill('0') << std::hex << object->tag()
                                << ": Skipping sub-IFDs beyond the first " << i << ".\n";
#endif
                    break;
                }
                // If there are multiple dirs, group is incremented for each
                TiffComponent::UniquePtr td(new TiffDirectory(object->tag(),
                                                            static_cast<IfdId>(object->newGroup_ + i)));
                td->setStart(pData_ + baseOffset() + offset);
                object->addChild(std::move(td));
            }
        }
#ifndef SUPPRESS_WARNINGS
        else {
            EXV_WARNING << "Directory " << groupName(object->group())
                        << ", entry 0x" << std::setw(4)
                        << std::setfill('0') << std::hex << object->tag()
                        << " doesn't look like a sub-IFD.\n";
        }
#endif

    } // TiffReader::visitSubIfd