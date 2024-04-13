    Auth::Info PamData::handleInfo(const struct pam_message* msg, bool predict) {
        if (QString::fromLocal8Bit(msg->msg).indexOf(QRegExp(QStringLiteral("^Changing password for [^ ]+$")))) {
            if (predict)
                m_currentRequest = Request(changePassRequest);
            return Auth::INFO_PASS_CHANGE_REQUIRED;
        }
        return Auth::INFO_UNKNOWN;
    }