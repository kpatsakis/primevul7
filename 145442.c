    void TiffReader::setMnState(const TiffRwState* state)
    {
        if (state != 0) {
            // invalidByteOrder indicates 'no change'
            if (state->byteOrder() == invalidByteOrder) {
                mnState_ = TiffRwState(origState_.byteOrder(), state->baseOffset());
            }
            else {
                mnState_ = *state;
            }
        }
        pState_ = &mnState_;
    }