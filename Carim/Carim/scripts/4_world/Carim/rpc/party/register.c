class CarimRPCPartyRegister extends CarimRPCBase<Param1<array<string>>> {
    override int GetType() {
        return CarimRPCTypesParty.REGISTER;
    }

    override void HandleClient(PlayerIdentity sender, Param1<array<string>> params) {
        MissionBaseWorld.Cast(GetGame().GetMission()).CarimPartyClientSetMutual(params.param1);
    }

    override void HandleServer(PlayerIdentity sender, Param1<array<string>> params) {
        MissionBaseWorld.Cast(GetGame().GetMission()).CarimPartyServerRegister(CarimUtil.GetIdentifier(sender), params.param1);
    }
}
