    void Display::slotAuthenticationFinished(const QString &user, bool success) {
        if (success) {
            qDebug() << "Authenticated successfully";

            m_auth->setCookie(qobject_cast<XorgDisplayServer *>(m_displayServer)->cookie());

            // save last user and last session
            if (mainConfig.Users.RememberLastUser.get())
                stateConfig.Last.User.set(m_auth->user());
            else
                stateConfig.Last.User.setDefault();
            if (mainConfig.Users.RememberLastSession.get())
                stateConfig.Last.Session.set(m_sessionName);
            else
                stateConfig.Last.Session.setDefault();
            stateConfig.save();

            // switch to the new VT for Wayland sessions
            if (m_lastSession.xdgSessionType() == QLatin1String("wayland"))
                // set vt_auto to false, so handle the vt switch yourself (VT_PROCESS)
                VirtualTerminal::jumpToVt(m_lastSession.vt(), false);

            if (m_socket)
                emit loginSucceeded(m_socket);
        } else if (m_socket) {
            qDebug() << "Authentication failure";
            emit loginFailed(m_socket);
        }
        m_socket = nullptr;
    }