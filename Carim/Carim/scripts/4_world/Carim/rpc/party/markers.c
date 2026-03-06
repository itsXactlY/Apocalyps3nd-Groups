class CarimRPCPartyMarkers extends CarimRPCBase<Param2<string, CarimModelPartyMarkers>> {
    override int GetType() {
        return CarimRPCTypesParty.MARKERS;
    }

    override void HandleClient(PlayerIdentity sender, Param2<string, CarimModelPartyMarkers> params) {
        MissionBaseWorld.Cast(GetGame().GetMission()).CarimPartyClientAddMarkers(params.param1, params.param2);
    }
}
