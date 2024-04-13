    void Display::start() {
        // check flag
        if (m_started)
            return;

        // start display server
        if (!m_displayServer->start()) {
            qFatal("Display server failed to start. Exiting");
        }
    }