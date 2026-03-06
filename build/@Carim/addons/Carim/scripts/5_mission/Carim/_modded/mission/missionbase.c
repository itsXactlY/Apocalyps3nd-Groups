modded class MissionBase {
    ref CarimModelSettings carimModelSettings = new CarimModelSettings;

    void MissionBase() {
        carimModelSettings.Load();
        CarimLogging.settings = carimModelSettings;
    }
}
