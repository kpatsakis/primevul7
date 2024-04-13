    AuthPrompt::Type PamData::detectPrompt(const struct pam_message* msg) const {
        if (msg->msg_style == PAM_PROMPT_ECHO_OFF) {
            QString message = QString::fromLocal8Bit(msg->msg);
            if (message.indexOf(QRegExp(QStringLiteral("\\bpassword\\b"), Qt::CaseInsensitive)) >= 0) {
                if (message.indexOf(QRegExp(QStringLiteral("\\b(re-?(enter|type)|again|confirm|repeat)\\b"), Qt::CaseInsensitive)) >= 0) {
                    return AuthPrompt::CHANGE_REPEAT;
                }
                else if (message.indexOf(QRegExp(QStringLiteral("\\bnew\\b"), Qt::CaseInsensitive)) >= 0) {
                    return AuthPrompt::CHANGE_NEW;
                }
                else if (message.indexOf(QRegExp(QStringLiteral("\\b(old|current)\\b"), Qt::CaseInsensitive)) >= 0) {
                    return AuthPrompt::CHANGE_CURRENT;
                }
                else {
                    return AuthPrompt::LOGIN_PASSWORD;
                }
            }
        }
        else {
            return AuthPrompt::LOGIN_USER;
        }

        return AuthPrompt::UNKNOWN;
    }