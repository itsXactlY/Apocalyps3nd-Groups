class CarimModelAbcBase extends Managed {
    void CarimModelAbcBase() {
        CarimLogging.Debug(this, "Spawned instance");
    }

    void Load();
    void Persist();
}
