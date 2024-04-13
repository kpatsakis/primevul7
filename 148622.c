    bool PamBackend::openSession() {
        if (!m_pam->setCred(PAM_ESTABLISH_CRED)) {
            m_app->error(m_pam->errorString(), Auth::ERROR_AUTHENTICATION);
            return false;
        }

        QProcessEnvironment sessionEnv = m_app->session()->processEnvironment();
        if (sessionEnv.value(QStringLiteral("XDG_SESSION_TYPE")) == QLatin1String("x11")) {
            QString display = sessionEnv.value(QStringLiteral("DISPLAY"));
            if (!display.isEmpty()) {
#ifdef PAM_XDISPLAY
                m_pam->setItem(PAM_XDISPLAY, qPrintable(display));
#endif
                m_pam->setItem(PAM_TTY, qPrintable(display));
            }
        } else if (sessionEnv.value(QStringLiteral("XDG_SESSION_TYPE")) == QLatin1String("wayland")) {
            QString tty = QStringLiteral("/dev/tty%1").arg(sessionEnv.value(QStringLiteral("XDG_VTNR")));
            m_pam->setItem(PAM_TTY, qPrintable(tty));
        }

        if (!m_pam->putEnv(sessionEnv)) {
            m_app->error(m_pam->errorString(), Auth::ERROR_INTERNAL);
            return false;
        }
        if (!m_pam->openSession()) {
            m_app->error(m_pam->errorString(), Auth::ERROR_INTERNAL);
            return false;
        }
        sessionEnv.insert(m_pam->getEnv());
        m_app->session()->setProcessEnvironment(sessionEnv);
        return Backend::openSession();
    }