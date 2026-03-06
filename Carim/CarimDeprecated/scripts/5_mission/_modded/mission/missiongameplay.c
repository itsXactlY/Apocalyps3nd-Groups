#ifndef CARIM_DEPRECATED_SINGLE_NOTIFICATION
#define CARIM_DEPRECATED_SINGLE_NOTIFICATION

modded class MissionGameplay {
    protected float carimDeprecatedLastUpdated = 420.0;

    override void OnUpdate(float timeslice) {
        super.OnUpdate(timeslice);

        if (carimDeprecatedLastUpdated > 600) {
            NotificationSystem.AddNotificationExtended(30, "ACTION REQUIRED", "The SchanaMod in use will be removed from the Workshop soon. All features are now in Carim (https://steamcommunity.com/sharedfiles/filedetails/?id=3468061029).");
            carimDeprecatedLastUpdated = 0.0;
        } else {
            carimDeprecatedLastUpdated += timeslice;
        }
    }
}

#endif
