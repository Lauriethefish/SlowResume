#include "main.hpp"
#include "GlobalNamespace/PauseAnimationController.hpp"
#include "GlobalNamespace/ScoreController.hpp"
#include "UnityEngine/Animator.hpp"
using namespace GlobalNamespace;

#include "custom-types/shared/register.hpp"
#include "questui/shared/QuestUI.hpp"
#include "bs-utils/shared/utils.hpp"

#include "SettingsViewController.hpp"
using namespace SlowResume;

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}

// Returns a logger, useful for printing debug messages
const Logger& getLogger() {
    static const Logger logger(modInfo);
    return logger;
}

// Creates a default config file if one does not already exist
void createDefaultConfig() {
    ConfigDocument& config = getConfig().config;
    if(!config.HasMember("speed")) {
        config.AddMember("speed", 1.0, config.GetAllocator());
    }
}

// Called whenever the resume button is pressed while the game is paused
MAKE_HOOK_OFFSETLESS(PauseAnimationController_StartResumeFromPauseAnimation, void, PauseAnimationController* self) {
    UnityEngine::Animator* animator = self->animator;
    float speed = getConfig().config["speed"].GetFloat();

    if(speed != 1.0) { // Only set the speed if it was not the default, just to be sure that we don't mess something up if disabled
        // Disable score submission if the amount of time was decreased
        if(speed < 1.0) {
            getLogger().info("Disabling score submission . . .");
            bs_utils::Submission::disable(modInfo);
        }

        // Set the speed of the resume animation
        getLogger().info("Setting pause animation speed to: " + std::to_string(speed));
        animator->set_speed(speed);
    }

    PauseAnimationController_StartResumeFromPauseAnimation(self);
}

// Manually re-enable score submission when scoring starts, because bs-utils isn't doing it automatically . . .
MAKE_HOOK_OFFSETLESS(ScoreController_Start, void, ScoreController* self) {
    getLogger().info("Re-enabling score submission . . .");
    bs_utils::Submission::enable(modInfo);
    ScoreController_Start(self);
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
	
    getConfig().Load(); // Load the config file
    createDefaultConfig();
    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();

    getLogger().info("Installing hooks...");

    QuestUI::Init(); // Register QuestUI types/other stuff
    custom_types::Register::RegisterType<SettingsViewController>(); // Register the custom ViewController for our mod settings menu

    QuestUI::Register::RegisterModSettingsViewController<SettingsViewController*>(modInfo); // Make QuestUI show it as an option in mod settings

    // Install our hooks
    INSTALL_HOOK_OFFSETLESS(PauseAnimationController_StartResumeFromPauseAnimation, 
                            il2cpp_utils::FindMethodUnsafe("", "PauseAnimationController", "StartResumeFromPauseAnimation", 0));
    INSTALL_HOOK_OFFSETLESS(ScoreController_Start, 
                            il2cpp_utils::FindMethodUnsafe("", "ScoreController", "Start", 0));
    getLogger().info("Installed all hooks!");
}