class CarimRPCChat extends CarimRPCBase<Param1<string>> {
    override int GetType() {
        return CarimRPCTypesChat.SEND;
    }

    override void HandleServer(PlayerIdentity sender, Param1<string> params) {
        auto outParam = new Param1<string>(sender.GetName() + " : " + params.param1);
        array<Man> players = {};
        GetGame().GetPlayers(players);
        foreach(Man player : players) {
            GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, outParam, true, player.GetIdentity());
        }
    }
}
