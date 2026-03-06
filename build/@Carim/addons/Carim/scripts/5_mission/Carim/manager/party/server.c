class CarimManagerPartyServer extends Managed {
    ref CarimModelPartyMarkers markers = new CarimModelPartyMarkers;

    ref CarimModelPartyParties parties;

    ref CarimRPCPartyMarkers rpcMarkers = new CarimRPCPartyMarkers;
    ref CarimRPCPartyPositions rpcPositions = new CarimRPCPartyPositions;
    ref CarimRPCPartyRegister rpcRegister = new CarimRPCPartyRegister;

    void CarimManagerPartyServer(CarimModelPartyParties iParties) {
        parties = iParties;
        // TODO: stagger this so it doesn't send to everyone at once
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.SendPositions, 10000, true);
    }

    void ~CarimManagerPartyServer() {
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.SendPositions);
    }

    void RegisterMarkers(string id, CarimModelPartyPings playerMarkers) {
        if (playerMarkers.markers.Contains(id)) {
            foreach(CarimMapMarker marker : playerMarkers.markers.Get(id)) {
                marker.CarimSetMarkerColor(CfgGameplayHandler.GetCarimPartyColorPingIconServer());
                marker.CarimSetMarkerIcon(CarimMapMarkerTypes.ARROW_2);
            }
            markers.Replace(id, playerMarkers.markers.Get(id));
        } else {
            markers.Clear(id);
        }

        // Send markers to mutual members
        if (parties.mutuals.Contains(id)) {
            // translate pings to markers
            // TODO: include map markers
            auto markersToSend = new CarimModelPartyMarkers;
            markersToSend.Replace(id, markers.markers.Get(id));
            auto idMap = CarimManagerPartyUtil.GetServerIdPlayerMap();
            auto mutualPlayers = parties.mutuals.Get(id).ToArray();
            foreach(string playerId : mutualPlayers) {
                if (idMap.Contains(playerId)) {
                    PlayerBase player = idMap.Get(playerId);
                    if (player.GetIdentity() && player.IsAlive()) {
                        auto params = new Param2<string, CarimModelPartyMarkers>(id, markersToSend);
                        rpcMarkers.Send(player, params, player.GetIdentity());
                    }
                }
            }
        }
    }

    void RegisterParty(string id, array<string> players) {
        string jsonPlayers;
        if (CarimLogging.TraceEnabled()) {
            JsonSerializer().WriteToString(players, false, jsonPlayers);
        }
        CarimLogging.Trace(this, "Register " + id + ", " + jsonPlayers);
        CarimLogging.Trace(this, "Before: " + parties.Repr());
        bool changed = parties.Register(id, players);
        CarimLogging.Trace(this, "After: " + parties.Repr());
        if (changed) {
            SendParty();
            SendPositions();
        }
    }

    void SendPositions() {
        map<string, PlayerBase> idMap = CarimManagerPartyUtil.GetServerIdPlayerMap();
        auto players = new CarimModelPartyPositions;

        // Harvest the relevant information
        foreach(string id, PlayerBase player : idMap) {
            CarimLogging.Trace(this, "PartyPositionServer Harvest " + id);
            auto playerInfo = CarimMapMarker.CarimNew(player.GetPosition(), player.GetIdentity().GetName(), CarimColor.HEALTH_WHITE, CarimMapMarkerTypes.HEALTH_0, id, player.GetHealthLevel());
            CarimLogging.Trace(this, "PartyPositionServer Harvested " + playerInfo.CarimRepr());
            players.Add(playerInfo);
        }

        // Send the information to each recipient's mutual party members
        auto ids = parties.mutuals.GetKeyArray();
        foreach(string recipient : ids) {
            if (idMap.Contains(recipient) && parties.mutuals.Contains(recipient)) {
                // TODO: clean up registered players that are no longer present
                auto positions = new CarimModelPartyPositions;
                auto mutualPlayers = parties.mutuals.Get(recipient).ToArray();
                foreach(string mutual : mutualPlayers) {
                    if (players.markers.Contains(mutual)) {
                        positions.Replace(mutual, players.markers.Get(mutual));
                    }
                }
                Param1<CarimModelPartyPositions> params = new Param1<CarimModelPartyPositions>(positions);
                rpcPositions.Send(idMap.Get(recipient), params, idMap.Get(recipient).GetIdentity());
            }
        }
    }

    void SendParty() {
        map<string, PlayerBase> idMap = CarimManagerPartyUtil.GetServerIdPlayerMap();
        foreach(string id, PlayerBase player : idMap) {
            array<string> mutuals = new array<string>;
            if (parties.mutuals.Contains(id)) {
                auto mutualPlayers = parties.mutuals.Get(id).ToArray();
                foreach(string mutual : mutualPlayers) {
                    mutuals.Insert(mutual);
                }
            }
            Param1<array<string>> params = new Param1<array<string>>(mutuals);
            rpcRegister.Send(player, params, player.GetIdentity());
        }
    }
}
