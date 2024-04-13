    void TiffReader::visitBinaryArray(TiffBinaryArray* object)
    {
        assert(object != 0);

        if (!postProc_) {
            // Defer reading children until after all other components are read, but
            // since state (offset) is not set during post-processing, read entry here
            readTiffEntry(object);
            object->iniOrigDataBuf();
            postList_.push_back(object);
            return;
        }

        // Check duplicates
        TiffFinder finder(object->tag(), object->group());
        pRoot_->accept(finder);
        TiffEntryBase* te = dynamic_cast<TiffEntryBase*>(finder.result());
        if (te && te->idx() != object->idx()) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Not decoding duplicate binary array tag 0x"
                        << std::setw(4) << std::setfill('0') << std::hex
                        << object->tag() << std::dec << ", group "
                        << groupName(object->group()) << ", idx " << object->idx()
                        << "\n";
#endif
            object->setDecoded(false);
            return;
        }

        if (object->TiffEntryBase::doSize() == 0) return;
        if (!object->initialize(pRoot_)) return;
        const ArrayCfg* cfg = object->cfg();
        if (cfg == 0) return;

        const CryptFct cryptFct = cfg->cryptFct_;
        if (cryptFct != 0) {
            const byte* pData = object->pData();
            int32_t size = object->TiffEntryBase::doSize();
            DataBuf buf = cryptFct(object->tag(), pData, size, pRoot_);
            if (buf.size_ > 0) object->setData(buf);
        }

        const ArrayDef* defs = object->def();
        const ArrayDef* defsEnd = defs + object->defSize();
        const ArrayDef* def = &cfg->elDefaultDef_;
        ArrayDef gap = *def;

        for (uint32_t idx = 0; idx < object->TiffEntryBase::doSize(); ) {
            if (defs) {
                def = std::find(defs, defsEnd, idx);
                if (def == defsEnd) {
                    if (cfg->concat_) {
                        // Determine gap-size
                        const ArrayDef* xdef = defs;
                        for (; xdef != defsEnd && xdef->idx_ <= idx; ++xdef) {}
                        uint32_t gapSize = 0;
                        if (xdef != defsEnd && xdef->idx_ > idx) {
                            gapSize = xdef->idx_ - idx;
                        }
                        else {
                            gapSize = object->TiffEntryBase::doSize() - idx;
                        }
                        gap.idx_ = idx;
                        gap.tiffType_ = cfg->elDefaultDef_.tiffType_;
                        gap.count_ = gapSize / cfg->tagStep();
                        if (gap.count_ * cfg->tagStep() != gapSize) {
                            gap.tiffType_ = ttUndefined;
                            gap.count_ = gapSize;
                        }
                        def = &gap;
                    }
                    else {
                        def = &cfg->elDefaultDef_;
                    }
                }
            }
            idx += object->addElement(idx, *def); // idx may be different from def->idx_
        }

    } // TiffReader::visitBinaryArray