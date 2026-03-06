class CarimModelPartyParties extends CarimModelAbcBase {
    ref map<string, ref CarimSet> registered;
    ref map<string, ref CarimSet> mutuals;
    ref array<string> admins;

    void CarimModelPartyParties() {
        admins = CfgGameplayHandler.GetCarimAdminIds();
        registered = new map<string, ref CarimSet>;
        mutuals = new map<string, ref CarimSet>;
    }

    bool Register(string id, array<string> players) {
        if (!registered.Contains(id)) {
            registered.Insert(id, new CarimSet);
        }

        bool added;
        CarimSet removed = registered.Get(id).Copy();

        foreach(string newPlayer : players) {
            removed.Remove(newPlayer);
            if (!registered.Get(id).Contains(newPlayer)) {
                CarimLogging.Info(this, "Register " + id + " added " + newPlayer);
                added = true;
                registered.Get(id).Insert(newPlayer);
            }
        }

        auto removedPlayers = removed.ToArray();
        foreach(string removedPlayer : removedPlayers) {
            CarimLogging.Info(this, "Register " + id + " removed " + removedPlayer);
            registered.Get(id).Remove(removedPlayer);
            UpdateMutual(removedPlayer);
        }

        bool changed = (added || removed.Count() > 0);
        if (changed) {
            UpdateMutual(id);
            auto registeredPlayers = registered.Get(id).ToArray();
            foreach(string player : registeredPlayers) {
                UpdateMutual(player);
            }
        }

        return changed;
    }

    void UpdateMutual(string id) {
        CarimLogging.Trace(this, "UpdateMutual " + id);
        if (!registered.Contains(id)) {
            CarimLogging.Trace(this, "No registration found");
            return;
        }
        if (!mutuals.Contains(id)) {
            mutuals.Insert(id, new CarimSet);
        } else {
            mutuals.Get(id).Clear();
        }
        auto registeredPlayers = registered.Get(id).ToArray();
        foreach(string player : registeredPlayers) {
            CarimLogging.Trace(this, "Checking if mutual: " + player);
            if (registered.Contains(player) && registered.Get(player).Contains(id)) {
                CarimLogging.Trace(this, "Mutual found: " + player);
                mutuals.Get(id).Insert(player);
            } else if (admins.Find(id) != -1) {
                CarimLogging.Info(this, "Mutual admin override: " + player);
                mutuals.Get(id).Insert(player);
            }
        }
    }

    string Repr() {
        string jsonRegistered;
        if (CarimLogging.TraceEnabled()) {
            JsonSerializer().WriteToString(registered, false, jsonRegistered);
        }
        string jsonMutuals;
        if (CarimLogging.TraceEnabled()) {
            JsonSerializer().WriteToString(mutuals, false, jsonMutuals);
        }
        return "PartyParties<" + jsonRegistered + ", " + jsonMutuals + ">";
    }
}
