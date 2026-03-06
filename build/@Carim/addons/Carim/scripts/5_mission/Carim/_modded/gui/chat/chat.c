modded class Chat {
    override void Add(ChatMessageEventParams params) {
        if (CarimEnabled.Chat()) {
            int channel = params.param1;
            if ((channel == CCDirect || channel == 0) && params.param3.IndexOf(CARIM_CHAT_PREFIX) == 0) {
                return;
            }
        }
        super.Add(params);
    }

    override void AddInternal(ChatMessageEventParams params) {
        if (CarimEnabled.Chat()) {
            int channel = params.param1;
            string rbeFrom = params.param2;
            string rbeText = params.param3;
            string radioFrom = rbeFrom;
            if (rbeFrom.Length() > 7) {
                radioFrom = rbeFrom.Substring(0, 7);
            }
            string radioText = rbeText;
            if (rbeText.Length() > 7) {
                radioText = rbeText.Substring(0, 7);
            }
            if ((radioFrom == "(Radio)" || radioText == "(Radio)") && radioText.Contains(CARIM_CHAT_PREFIX)) {
                return;
            }
        }
        super.AddInternal(params);
    }

    void CarimUpdateSize() {
        foreach(ChatLine line : m_Lines) {
            line.CarimUpdateSize();
        }
    }
}
