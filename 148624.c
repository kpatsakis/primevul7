    PamBackend::PamBackend(HelperApp *parent)
            : Backend(parent)
            , m_data(new PamData())
            , m_pam(new PamHandle(this)) {
    }