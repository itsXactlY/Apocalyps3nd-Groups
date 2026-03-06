class CarimRPCPartyPositions extends CarimRPCBase<Param1<CarimModelPartyPositions>> {
    override int GetType() {
        return CarimRPCTypesParty.POSITIONS;
    }

    override void HandleClient(PlayerIdentity sender, Param1<CarimModelPartyPositions> params) {
        MissionBaseWorld.Cast(GetGame().GetMission()).CarimPartyClientSetPositions(params.param1);
    }
}
