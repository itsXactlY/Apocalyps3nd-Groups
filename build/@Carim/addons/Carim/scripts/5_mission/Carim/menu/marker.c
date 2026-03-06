class CarimMenuMarker extends UIScriptedMenu {
    protected ref CarimMapMarker carimMarker;
    protected int carimTextColor;
    protected int carimHideLessThan;
    protected int carimHideGreaterThan;
    protected bool carimShowDistance;
    protected bool carimRequireLineOfSight;

    protected TextWidget carimNametag;
    protected TextWidget carimDistance;
    protected ImageWidget carimIcon;

    protected float carimLastUpdated = 0.0;

    void CarimMenuMarker(CarimMapMarker marker, int textColor, int hideLessThan, int hideGreaterThan, bool showDistance, bool requireLineOfSight) {
        carimMarker = marker;
        carimTextColor = textColor;
        carimHideLessThan = hideLessThan;
        carimHideGreaterThan = hideGreaterThan;
        carimShowDistance = showDistance;
        carimRequireLineOfSight = requireLineOfSight;
    }

    void ~CarimMenuMarker() {
        if (layoutRoot) {
            layoutRoot.Unlink();
        }
    }

    void CarimSetAttributes(CarimMapMarker marker, int textColor, int hideLessThan, int hideGreaterThan, bool showDistance, bool requireLineOfSight) {
        carimMarker = marker;
        carimTextColor = textColor;
        carimHideLessThan = hideLessThan;
        carimHideGreaterThan = hideGreaterThan;
        carimShowDistance = showDistance;
        carimRequireLineOfSight = requireLineOfSight;
    }

    string CarimGetLayout() {
        return "Carim/Carim/gui/layouts/party/marker.layout";
    }

    override Widget Init() {
        if (!layoutRoot) {
            layoutRoot = Widget.Cast(GetGame().GetWorkspace().CreateWidgets(CarimGetLayout()));
            carimNametag = TextWidget.Cast(layoutRoot.FindAnyWidget("nametag"));
            carimDistance = TextWidget.Cast(layoutRoot.FindAnyWidget("distance"));
            carimIcon = ImageWidget.Cast(layoutRoot.FindAnyWidget("icon"));
        }

        return layoutRoot;
    }

    override void Update(float timeslice) {
        super.Update(timeslice);

        if (layoutRoot) {
            CarimSetRootPosition();

            if (carimLastUpdated > CARIM_60_FPS_INTERVAL_SEC) {
                CarimSetVisibility();
                CarimUpdateContent();
                carimLastUpdated = 0.0;
            } else {
                carimLastUpdated += timeslice;
            }
        }
    }

    void CarimSetRootPosition() {
        vector screenPosition = GetGame().GetScreenPos(carimMarker.GetMarkerPos() + CarimGetMarkerOffset());
        float x = screenPosition[0];
        float y = screenPosition[1];

        layoutRoot.SetPos(x, y);
    }

    vector CarimGetMarkerOffset() {
        if (carimMarker.carimPlayer) {
            return "0 1.3 0";
        } else {
            return "0 0.2 0";
        }
    }

    void CarimSetVisibility() {
        Mission mission = GetGame().GetMission();
        IngameHud hud = IngameHud.Cast(mission.GetHud());
        bool hudHid = hud && hud.GetHudVisibility().IsContextFlagActive(IngameHudVisibility.HUD_HIDE_FLAGS);
        carimDistance.Show(carimShowDistance);
        layoutRoot.Show(!hudHid && CarimVisibleOnScreen() && carimMarker.carimVisible3d);
    }

    bool CarimVisibleOnScreen() {
        vector pos = GetGame().GetScreenPosRelative(carimMarker.GetMarkerPos());
        if (pos[0] >= 1 || pos[0] == 0 || pos[1] >= 1 || pos[1] == 0) {
            return false;
        } else if (pos[2] < 0) {
            return false;
        }

        if (carimRequireLineOfSight) {
            auto player = GetGame().GetPlayer();
            if (player) {
                vector from = GetGame().GetCurrentCameraPosition();
                vector to = carimMarker.GetMarkerPos();
                vector contactPos;
                vector contactDir;
                int contactComponent;
                bool hit = DayZPhysics.RaycastRV(from, to, contactPos, contactDir, contactComponent, NULL, NULL, player, false, false, ObjIntersectIFire);
                // Check if contactPos is within 1m of marker position, and count that as not hit
                if (hit && vector.Distance(to, contactPos) > 1) {
                    return false;
                }
            }
        }

        return CarimVisibleDistance();
    }

    bool CarimVisibleDistance() {
        bool canSeeBasedOnDistance = true;
        auto distance = CarimGetDistance();
        if (carimHideLessThan >= 0 && distance <= carimHideLessThan) {
            canSeeBasedOnDistance = false;
        }
        if (carimHideGreaterThan >= 0 && distance >= carimHideGreaterThan) {
            canSeeBasedOnDistance = false;
        }
        if (carimMarker.carimHideGreaterThan >= 0 && distance >= carimMarker.carimHideGreaterThan) {
            canSeeBasedOnDistance = false;
        }
        return canSeeBasedOnDistance;
    }

    void CarimUpdateContent() {
        carimNametag.SetText(carimMarker.GetMarkerText());
        carimDistance.SetText(CarimFormatDistance(CarimGetDistance()));
        string imageFile = MapMarkerTypes.GetMarkerTypeFromID(carimMarker.GetMarkerIcon());
        imageFile.Replace("\\DZ", "DZ");
        carimIcon.LoadImageFile(0, imageFile);

        carimIcon.SetColor(carimMarker.GetMarkerColor());
        carimNametag.SetColor(carimTextColor);
        carimDistance.SetColor(carimTextColor);
        CarimOnUpdate();
    }

    float CarimGetDistance() {
        auto player = GetGame().GetPlayer();
        if (player) {
            return Math.Round(vector.Distance(carimMarker.GetMarkerPos(), player.GetPosition()));
        }
        return -1;
    }

    string CarimFormatDistance(float distance) {
        if (distance < 0) {
            return "";
        }
        string distanceString = distance.ToString() + "m";
        if (distance > 1000) {
            distanceString = (Math.Round(distance / 100) / 10).ToString() + "km";
        }
        return distanceString;
    }

    void CarimOnUpdate();
}
