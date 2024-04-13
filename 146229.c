    void TiffReader::visitDirectory(TiffDirectory* object)
    {
        assert(object != 0);

        const byte* p = object->start();
        assert(p >= pData_);

        if (circularReference(object->start(), object->group())) return;

        if (p + 2 > pLast_) {
#ifndef SUPPRESS_WARNINGS
            EXV_ERROR << "Directory " << groupName(object->group())
                      << ": IFD exceeds data buffer, cannot read entry count.\n";
#endif
            return;
        }
        const uint16_t n = getUShort(p, byteOrder());
        p += 2;
        // Sanity check with an "unreasonably" large number
        if (n > 256) {
#ifndef SUPPRESS_WARNINGS
            EXV_ERROR << "Directory " << groupName(object->group()) << " with "
                      << n << " entries considered invalid; not read.\n";
#endif
            return;
        }
        for (uint16_t i = 0; i < n; ++i) {
            if (p + 12 > pLast_) {
#ifndef SUPPRESS_WARNINGS
                EXV_ERROR << "Directory " << groupName(object->group())
                          << ": IFD entry " << i
                          << " lies outside of the data buffer.\n";
#endif
                return;
            }
            uint16_t tag = getUShort(p, byteOrder());
            TiffComponent::UniquePtr tc = TiffCreator::create(tag, object->group());
            if (tc.get()) {
                tc->setStart(p);
                object->addChild(std::move(tc));
            } else {
               EXV_WARNING << "Unable to handle tag " << tag << ".\n";
            }
            p += 12;
        }

        if (object->hasNext()) {
            if (p + 4 > pLast_) {
#ifndef SUPPRESS_WARNINGS
                EXV_ERROR << "Directory " << groupName(object->group())
                          << ": IFD exceeds data buffer, cannot read next pointer.\n";
#endif
                return;
            }
            TiffComponent::UniquePtr tc;
            uint32_t next = getLong(p, byteOrder());
            if (next) {
                tc = TiffCreator::create(Tag::next, object->group());
#ifndef SUPPRESS_WARNINGS
                if (tc.get() == 0) {
                    EXV_WARNING << "Directory " << groupName(object->group())
                                << " has an unexpected next pointer; ignored.\n";
                }
#endif
            }
            if (tc.get()) {
                if (baseOffset() + next > size_) {
#ifndef SUPPRESS_WARNINGS
                    EXV_ERROR << "Directory " << groupName(object->group())
                              << ": Next pointer is out of bounds; ignored.\n";
#endif
                    return;
                }
                tc->setStart(pData_ + baseOffset() + next);
                object->addNext(std::move(tc));
            }
        } // object->hasNext()

    } // TiffReader::visitDirectory