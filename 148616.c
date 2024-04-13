    PamBackend::~PamBackend() {
        delete m_data;
        delete m_pam;
    }