    Prompt& PamData::findPrompt(const struct pam_message* msg) {
        AuthPrompt::Type type = detectPrompt(msg);

        for (Prompt &p : m_currentRequest.prompts) {
            if (type == AuthPrompt::UNKNOWN && QString::fromLocal8Bit(msg->msg) == p.message)
                return p;
            if (type == p.type)
                return p;
        }

        return invalidPrompt;
    }