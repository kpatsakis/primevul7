    bool PamData::insertPrompt(const struct pam_message* msg, bool predict) {
        Prompt &p = findPrompt(msg);

        // first, check if we already have stored this propmpt
        if (p.valid()) {
            // we have a response already - do nothing
            if (m_sent)
                return false;
            // we don't have a response yet - replace the message and prepare to send it
            p.message = QString::fromLocal8Bit(msg->msg);
            return true;
        }
        // this prompt is not stored but we have some prompts
        else if (m_currentRequest.prompts.length() != 0) {
            // check if we have already sent this - if we did, get rid of the answers
            if (m_sent) {
                m_currentRequest.clear();
                m_sent = false;
            }
        }

        // we'll predict what will come next
        if (predict) {
            AuthPrompt::Type type = detectPrompt(msg);
            switch (type) {
                case AuthPrompt::LOGIN_USER:
                    m_currentRequest = Request(loginRequest);
                    return true;
                case AuthPrompt::CHANGE_CURRENT:
                    m_currentRequest = Request(changePassRequest);
                    return true;
                case AuthPrompt::CHANGE_NEW:
                    m_currentRequest = Request(changePassNoOldRequest);
                    return true;
                default:
                    break;
            }
        }

        // or just add whatever comes exactly as it comes
        m_currentRequest.prompts.append(Prompt(detectPrompt(msg), QString::fromLocal8Bit(msg->msg), msg->msg_style == PAM_PROMPT_ECHO_OFF));

        return true;
    }