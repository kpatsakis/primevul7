    int FileIo::Impl::switchMode(OpMode opMode)
    {
        if (fp_ == nullptr)
            return 1;
        if (opMode_ == opMode)
            return 0;
        OpMode oldOpMode = opMode_;
        opMode_ = opMode;

        bool reopen = true;
        switch (opMode) {
            case opRead:
                // Flush if current mode allows reading, else reopen (in mode "r+b"
                // as in this case we know that we can write to the file)
                if (openMode_[0] == 'r' || openMode_[1] == '+')
                    reopen = false;
                break;
            case opWrite:
                // Flush if current mode allows writing, else reopen
                if (openMode_[0] != 'r' || openMode_[1] == '+')
                    reopen = false;
                break;
            case opSeek:
                reopen = false;
                break;
        }

        if (!reopen) {
            // Do not do anything when switching _from_ opSeek mode; we
            // flush when switching _to_ opSeek.
            if (oldOpMode == opSeek)
                return 0;

            // Flush. On msvcrt fflush does not do the job
            std::fseek(fp_, 0, SEEK_CUR);
            return 0;
        }

        // Reopen the file
        long offset = std::ftell(fp_);
        if (offset == -1)
            return -1;
        // 'Manual' open("r+b") to avoid munmap()
        std::fclose(fp_);
        openMode_ = "r+b";
        opMode_ = opSeek;
#ifdef EXV_UNICODE_PATH
        if (wpMode_ == wpUnicode) {
            fp_ = ::_wfopen(wpath_.c_str(), s2ws(openMode_).c_str());
        } else
#endif
        {
            fp_ = std::fopen(path_.c_str(), openMode_.c_str());
        }
        if (fp_ == nullptr)
            return 1;
        return std::fseek(fp_, offset, SEEK_SET);
    }  // FileIo::Impl::switchMode