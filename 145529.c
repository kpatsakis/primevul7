    size_t RemoteIo::write(BasicIo& src)
    {
        assert(p_->isMalloced_);
        if (!src.isopen()) return 0;

        /*
         * The idea is to compare the file content, find the different bytes and submit them to the remote machine.
         * To simplify it, it:
         *      + goes from the left, find the first different position -> $left
         *      + goes from the right, find the first different position -> $right
         * The different bytes are [$left-$right] part.
         */
        size_t left       = 0;
        size_t right      = 0;
        size_t blockIndex = 0;
        size_t i          = 0;
        size_t readCount  = 0;
        size_t blockSize  = 0;
        byte*  buf        = (byte*) std::malloc(p_->blockSize_);
        size_t nBlocks    = (p_->size_ + p_->blockSize_ - 1) / p_->blockSize_;

        // find $left
        src.seek(0, BasicIo::beg);
        bool findDiff = false;
        while (blockIndex < nBlocks && !src.eof() && !findDiff) {
            blockSize = p_->blocksMap_[blockIndex].getSize();
            bool isFakeData = p_->blocksMap_[blockIndex].isKnown(); // fake data
            readCount = (size_t) src.read(buf, (long) blockSize);
            byte* blockData = p_->blocksMap_[blockIndex].getData();
            for (i = 0; (i < readCount) && (i < blockSize) && !findDiff; i++) {
                if ((!isFakeData && buf[i] != blockData[i]) || (isFakeData && buf[i] != 0)) {
                    findDiff = true;
                } else {
                    left++;
                }
            }
            blockIndex++;
        }

        // find $right
        findDiff    = false;
        blockIndex  = nBlocks - 1;
        blockSize   = p_->blocksMap_[blockIndex].getSize();
        while ((blockIndex + 1 > 0) && right < src.size() && !findDiff) {
            if(src.seek(-1 * (blockSize + right), BasicIo::end)) {
                findDiff = true;
            } else {
                bool isFakeData = p_->blocksMap_[blockIndex].isKnown(); // fake data
                readCount = src.read(buf, (long) blockSize);
                byte* blockData = p_->blocksMap_[blockIndex].getData();
                for (i = 0; (i < readCount) && (i < blockSize) && !findDiff; i++) {
                    if ((!isFakeData && buf[readCount - i - 1] != blockData[blockSize - i - 1]) || (isFakeData && buf[readCount - i - 1] != 0)) {
                        findDiff = true;
                    } else {
                        right++;
                    }
                }
            }
            blockIndex--;
            blockSize = (long)p_->blocksMap_[blockIndex].getSize();
        }

        // free buf
        if (buf) std::free(buf);

        // submit to the remote machine.
        long dataSize = (long) (src.size() - left - right);
        if (dataSize > 0) {
            byte* data = (byte*) std::malloc(dataSize);
            src.seek(left, BasicIo::beg);
            src.read(data, dataSize);
            p_->writeRemote(data, (size_t)dataSize, (long)left, (long) (p_->size_ - right));
            if (data) std::free(data);
        }
        return (long) src.size();
    }