modded class ChatLine {
    void ChatLine(Widget root_widget) {
        if (CarimEnabled.Chat()) {
            m_RootWidget = GetGame().GetWorkspace().CreateWidgets("Carim/Carim/gui/layouts/chatline.layout", root_widget);
            m_NameWidget = TextWidget.Cast(m_RootWidget.FindAnyWidget("ChatItemSenderWidget"));
            m_TextWidget = TextWidget.Cast(m_RootWidget.FindAnyWidget("ChatItemTextWidget"));
            auto carimSettings = MissionGameplay.Cast(GetGame().GetMission()).carimModelChatSettings;
            if (carimSettings) {
                m_NameWidget.SetTextExactSize(carimSettings.size);
                m_TextWidget.SetTextExactSize(carimSettings.size);
            }
        }
    }

    override void Set(ChatMessageEventParams params) {
        super.Set(params);
        if (CarimEnabled.Chat()) {
            int channel = params.param1;
            if (channel & CCSystem) {
                if (params.param2 == "" && params.param3.IndexOf(" : ") > 0) {
                    CarimSetColour(CfgGameplayHandler.GetCarimChatColorGlobal());
                } else {
                    SetColour(CfgGameplayHandler.GetCarimChatColorAlert());
                }
            } else if (channel & CCAdmin) {
                SetColour(CfgGameplayHandler.GetCarimChatColorServer());
            } else if (channel == 0 || channel & CCDirect) {
                CarimSetColour(CfgGameplayHandler.GetCarimChatColorDirect());
            } else {
                SetColour(CfgGameplayHandler.GetCarimChatColorServer());
            }
        }
    }

    void CarimUpdateSize() {
        auto carimSettings = MissionGameplay.Cast(GetGame().GetMission()).carimModelChatSettings;
        if (carimSettings) {
            m_NameWidget.SetTextExactSize(carimSettings.size);
            m_TextWidget.SetTextExactSize(carimSettings.size);
        }
    }

    void CarimSetColour(int colour) {
        m_NameWidget.SetColor(colour);
        m_TextWidget.SetColor(colour);
    }
}
