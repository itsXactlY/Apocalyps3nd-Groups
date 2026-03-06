class CfgPatches {
    class Carim {
        requiredAddons[] = {"DZ_Scripts"};
    };
};

class CfgMods {
    class Carim {
        name = "Carim";
        action = "https://github.com/CarimDayZ/dayz-mod-carim";
        author = "cnofafva";
        type = "mod";
        inputs = "Carim/inputs.xml";
        dependencies[] = {"Game", "World", "Mission"};

        class defs {
            class gameScriptModule {
                files[] = {
                    "Carim/scripts/3_game",
                };
            };

            class worldScriptModule {
                files[] = {
                    "Carim/scripts/4_world",
                };
            };

            class missionScriptModule {
                files[] = {
                    "Carim/scripts/5_mission",
                };
            };
        };
    };
};
