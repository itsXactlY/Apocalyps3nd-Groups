modded class MapMarkerTypes {
    static int carimAdditionalIndex = 0;

    static void CarimInit() {
        CarimLogging.Trace(null, "MapMarkerTypes CarimInit");
        RegisterMarkerType(CarimMapMarkerTypes.ARROW_1, "set:dayz_gui image:iconArrowUp1");
        RegisterMarkerType(CarimMapMarkerTypes.ARROW_2, "set:dayz_gui image:iconArrowUp2");
        RegisterMarkerType(CarimMapMarkerTypes.ARROW_3, "set:dayz_gui image:iconArrowUp3");
        RegisterMarkerType(CarimMapMarkerTypes.HEALTH_0, "set:dayz_gui image:iconHealth0");
        RegisterMarkerType(CarimMapMarkerTypes.HEALTH_1, "set:dayz_gui image:iconHealth1");
        RegisterMarkerType(CarimMapMarkerTypes.HEALTH_2, "set:dayz_gui image:iconHealth2");
        RegisterMarkerType(CarimMapMarkerTypes.HEALTH_3, "set:dayz_gui image:iconHealth3");
        RegisterMarkerType(CarimMapMarkerTypes.HEALTH_4, "set:dayz_gui image:iconHealth4");

        array<string> carimMapIcons = {
            "\\DZ\\gear\\navigation\\data\\map_bunker_ca.paa",
            "\\DZ\\gear\\navigation\\data\\map_bush_ca.paa",
            "\\DZ\\gear\\navigation\\data\\map_busstop_ca.paa",
            "\\DZ\\gear\\navigation\\data\\map_chapel_ca.paa",
            "\\DZ\\gear\\navigation\\data\\map_church_ca.paa",
            "\\DZ\\gear\\navigation\\data\\map_cross_ca.paa",
            "\\DZ\\gear\\navigation\\data\\map_fountain_ca.paa",
            "\\DZ\\gear\\navigation\\data\\map_fuelstation_ca.paa",
            "\\DZ\\gear\\navigation\\data\\map_hospital_ca.paa",
            "\\DZ\\gear\\navigation\\data\\map_lighthouse_ca.paa",
            "\\DZ\\gear\\navigation\\data\\map_quay_ca.paa",
            "\\DZ\\gear\\navigation\\data\\map_rock_ca.paa",
            "\\DZ\\gear\\navigation\\data\\map_ruin_ca.paa",
            "\\DZ\\gear\\navigation\\data\\map_shipwreck_ca.paa",
            "\\DZ\\gear\\navigation\\data\\map_smalltree_ca.paa",
            "\\DZ\\gear\\navigation\\data\\map_stack_ca.paa",
            "\\DZ\\gear\\navigation\\data\\map_tree_ca.paa",
            "\\DZ\\gear\\navigation\\data\\map_viewtower_ca.paa",
            "\\DZ\\gear\\navigation\\data\\map_waterspring_ca.paa",
            "\\DZ\\gear\\navigation\\data\\map_watertower_ca.paa",
        };

        foreach(string markerAsset : carimMapIcons) {
            CarimLogging.Trace(null, string.Format("Register additional icon %1 %2", eMapMarkerTypes.MARKERTYPE_MAX + carimAdditionalIndex, markerAsset));
            RegisterMarkerType(eMapMarkerTypes.MARKERTYPE_MAX + carimAdditionalIndex, markerAsset);
            ++carimAdditionalIndex;
        }

        carimMapIcons = CfgGameplayHandler.GetCarimMapIcons();
        foreach(string configuredMarkerAsset : carimMapIcons) {
            CarimLogging.Trace(null, string.Format("Register configured icon %1 %2", eMapMarkerTypes.MARKERTYPE_MAX + carimAdditionalIndex, configuredMarkerAsset));
            RegisterMarkerType(eMapMarkerTypes.MARKERTYPE_MAX + carimAdditionalIndex, configuredMarkerAsset);
            ++carimAdditionalIndex;
        }
    }
}
