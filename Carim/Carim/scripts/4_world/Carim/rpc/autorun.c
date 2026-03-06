class CarimRPCAutorun extends CarimRPCBase<Param3<bool, int, int>> {
    override int GetType() {
        return CarimRPCTypesAutorun.AUTORUN;
    }

    override void HandleServer(PlayerIdentity sender, Param3<bool, int, int> params) {
        bool isRunning = params.param1;
        int movementIdx = params.param2;
        int angle = params.param3;
        PlayerBase player = PlayerBase.Cast(sender.GetPlayer());
        player.GetInputController().OverrideMovementSpeed(isRunning, movementIdx);
        player.GetInputController().OverrideMovementAngle(isRunning, angle);
    }
}
