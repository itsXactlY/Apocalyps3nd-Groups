modded class MissionBaseWorld {
    void CarimPartyClientAddMarkers(string id, CarimModelPartyMarkers markers);
    void CarimPartyClientSetPositions(CarimModelPartyPositions positions);
    void CarimPartyClientSetMutual(array<string> ids);
    void CarimPartyServerRegister(string id, array<string> ids);
    void CarimPartyServerAddPings(string id, CarimModelPartyPings pings);
}
