modded class MissionServer {
    ref CarimManagerPartyServer carimManagerPartyServer;
    ref CarimModelPartyParties carimModelPartyParties;

    override void OnGameplayDataHandlerLoad() {
        super.OnGameplayDataHandlerLoad();

        CarimEnabled.Initialize();

        if (CarimEnabled.Party() && !carimManagerPartyServer) {
            carimModelPartyParties = new CarimModelPartyParties;
            carimManagerPartyServer = new CarimManagerPartyServer(carimModelPartyParties);
        }
    }

    override void CarimPartyServerAddPings(string id, CarimModelPartyPings pings) {
        if (carimManagerPartyServer) {
            carimManagerPartyServer.RegisterMarkers(id, pings);
        }
    }

    override void CarimPartyServerRegister(string id, array<string> ids) {
        if (carimManagerPartyServer) {
            carimManagerPartyServer.RegisterParty(id, ids);
        }
    }
}
