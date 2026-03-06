class CarimMenuPartyList extends CarimMenuMarker {
    int carimListIndex = 0;

    override string CarimGetLayout() {
        return "Carim/Carim/gui/layouts/party/list.layout";
    }

    override void CarimSetRootPosition() {
        float width, height, x, y;
        layoutRoot.GetSize(width, height);
        layoutRoot.GetPos(x, y);
        y = (2 + height) * carimListIndex;
        layoutRoot.SetPos(x, y);
    }

    override bool CarimVisibleOnScreen() {
        return true;
    }
}
