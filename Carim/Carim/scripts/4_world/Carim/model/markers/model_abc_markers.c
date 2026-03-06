class CarimModelAbcMarkers extends CarimModelAbcDiskJson {
    [NonSerialized()] bool changed = false;

    ref map<string, ref array<ref CarimMapMarker>> markers = new map<string, ref array<ref CarimMapMarker>>;

    void Replace(string id, array<ref CarimMapMarker> marks) {
        markers.Set(id, marks);
        Persist();
        changed = true;
    }

    void Add(CarimMapMarker mark) {
        if (!markers.Contains(mark.carimPlayerId)) {
            markers.Insert(mark.carimPlayerId, new array<ref CarimMapMarker>);
        }
        markers.Get(mark.carimPlayerId).Insert(mark);
        Persist();
        changed = true;
    }

    void Remove(CarimMapMarker mark) {
        int closest = GetClosestIndex(mark);

        if (closest >= 0) {
            markers.Get(mark.carimPlayerId).RemoveOrdered(closest);
            Persist();
            changed = true;
        }
    }

    CarimMapMarker GetClosest(CarimMapMarker mark) {
        int closest = GetClosestIndex(mark);

        if (closest >= 0) {
            return markers.Get(mark.carimPlayerId).Get(closest);
        }

        return null;
    }

    int GetClosestIndex(CarimMapMarker mark) {
        if (!markers.Contains(mark.carimPlayerId)) {
            return -1;
        }

        int closest = -1;
        int closestDistance = 50;

        foreach(int i, auto marker : markers.Get(mark.carimPlayerId)) {
            auto distance = vector.Distance(mark.GetMarkerPos(), marker.GetMarkerPos());

            if (distance < closestDistance) {
                closest = i;
                closestDistance = distance;
            }
        }

        return closest;
    }

    void Clear(string id) {
        if (markers.Contains(id)) {
            markers.Get(id).Resize(0);
        } else {
            markers.Insert(id, new array<ref CarimMapMarker>);
        }
        Persist();
        changed = true;
    }
}
