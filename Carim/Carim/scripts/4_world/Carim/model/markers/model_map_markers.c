class CarimModelMapMarkers extends CarimModelAbcMarkers {
    int counter = 0;

    override void Clear(string id) {
        counter = 0;
        super.Clear(id);
    }

    override void Add(CarimMapMarker mark) {
        mark.CarimSetMarkerText(mark.GetMarkerText() + counter.ToString());
        ++counter;
        super.Add(mark);
    }
}
