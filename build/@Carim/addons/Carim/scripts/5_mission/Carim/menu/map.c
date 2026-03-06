class CarimMenuMap extends MapMenu {
    float carimLastUpdated = 0.0;

    override Widget Init() {
        super.Init();
        LoadMapMarkers();
        return layoutRoot;
    }

    override void Update(float timeslice) {
        super.Update(timeslice);

        if (layoutRoot) {
            if (carimLastUpdated > CARIM_4_FPS_INTERVAL_SEC) {
                m_MapWidgetInstance.ClearUserMarks();
                LoadMapMarkers();
                carimLastUpdated = 0.0;
            } else {
                carimLastUpdated += timeslice;
            }
        }
    }

    override void LoadMapMarkers() {
        auto mission = MissionGameplay.Cast(GetGame().GetMission());
        CarimLoadMapMarkers(mission.carimModelMapMarkers);
        CarimLoadMapMarkers(mission.carimModelPartyPings);
        CarimLoadMapMarkers(mission.carimModelPartyMarkers);
        CarimLoadMapMarkers(mission.carimModelPartyPositions);
        CarimLoadMapMarkersWithOverrides(mission.carimModelStaticMarkers, mission.carimModelStaticOverrideMarkers);
    }

    void CarimLoadMapMarkers(CarimModelAbcMarkers markers) {
        CarimLoadMapMarkersWithOverrides(markers, null);
    }

    void CarimLoadMapMarkersWithOverrides(CarimModelAbcMarkers markers, CarimModelAbcMarkers overrides) {
        foreach(array<ref CarimMapMarker> markerArray : markers.markers) {
            foreach(CarimMapMarker marker : markerArray) {
                auto markerToAdd = marker;
                // Check if override is present
                if (overrides) {
                    auto potentialOverride = overrides.GetClosest(marker);
                    if (potentialOverride && potentialOverride.GetMarkerPos() == marker.GetMarkerPos()) {
                        markerToAdd = potentialOverride;
                    }
                }
                m_MapWidgetInstance.AddUserMark(markerToAdd.GetMarkerPos(), markerToAdd.GetMarkerText(), markerToAdd.GetMarkerColor(), MapMarkerTypes.GetMarkerTypeFromID(markerToAdd.GetMarkerIcon()));
            }
        }
    }

    override void CloseMapMenu() {
        CarimLogging.Trace(this, "CloseMapMenu");
        // setting this flag to false makes sure they aren't
        // synced to the map item
        m_WasChanged = false;

        bool isClosedWithShortcut = CfgGameplayHandler.GetMapIgnoreMapOwnership() && GetUApi().GetInputByID(UAMapToggle).LocalPress();
        if (isClosedWithShortcut && m_MapMenuHandler && m_MapMenuHandler.carimMenuEditMarker && m_MapMenuHandler.carimMenuEditMarker.visible) {
            return;
        }

        super.CloseMapMenu();
    }
}
