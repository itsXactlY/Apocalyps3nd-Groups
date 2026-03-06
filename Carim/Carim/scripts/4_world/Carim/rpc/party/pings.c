class CarimRPCPartyPings extends CarimRPCBase<Param2<string, CarimModelPartyPings>> {
    override int GetType() {
        return CarimRPCTypesParty.PINGS;
    }

    override void HandleServer(PlayerIdentity sender, Param2<string, CarimModelPartyPings> params) {
        MissionBaseWorld.Cast(GetGame().GetMission()).CarimPartyServerAddPings(CarimUtil.GetIdentifier(sender), params.param2);
    }
}
