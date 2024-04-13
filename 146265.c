    TiffComponent* TiffMnCreator::create(uint16_t           tag,
                                         IfdId              group,
                                         IfdId              mnGroup)
    {
        TiffComponent* tc = 0;
        const TiffMnRegistry* tmr = find(registry_, mnGroup);
        if (tmr) {

            if (tmr->newMnFct2_ == 0) {

                std::cout << "mnGroup = " << mnGroup << "\n";

            }

            assert(tmr->newMnFct2_);
            tc = tmr->newMnFct2_(tag, group, mnGroup);
        }
        return tc;
    } // TiffMnCreator::create