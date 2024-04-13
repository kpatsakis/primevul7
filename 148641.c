    void Display::slotAuthError(const QString &message, Auth::Error error) {
        // TODO: handle more errors
        qWarning() << "Authentication error:" << message;

        if (!m_socket)
            return;

        if (error == Auth::ERROR_AUTHENTICATION)
            emit loginFailed(m_socket);
    }