#include "SettingsViewController.hpp"
using namespace SlowResume;

#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/Events/UnityAction_1.hpp"
using namespace UnityEngine::UI;
using namespace UnityEngine::Events;

#include "TMPro/TextMeshProUGUI.hpp"
using namespace TMPro;

#include "questui/shared/BeatSaberUI.hpp"
using namespace QuestUI;

DEFINE_CLASS(SettingsViewController);

// Sets the correct message for if score submission will/won't be disabled.
void setScoreSubmissionText(TextMeshProUGUI* text) {
    float currentSpeed = getConfig().config["speed"].GetFloat();
    if(currentSpeed < 1.0) {
        text->set_text(il2cpp_utils::createcsstr("Score submission will be disabled, as the pause time has been decreased."));
        text->set_color(UnityEngine::Color::get_red());
    }   else    {
        text->set_text(il2cpp_utils::createcsstr("Score submission will not be disabled"));
        text->set_color(UnityEngine::Color::get_green());
    }
}

// Called whenever the animation speed setting is increased/decreased
void onResumeAnimationSpeedSettingChange(TextMeshProUGUI* scoreSubmissionText, float newValue) {
    getLogger().info("Updating speed to " + std::to_string(newValue));
    getConfig().config["speed"].SetFloat(newValue); // Set the new value in the config
    setScoreSubmissionText(scoreSubmissionText); // Update the score submission text
}

void SettingsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    if(!(addedToHierarchy && firstActivation)) {return;} // Only create the UI once

    getLogger().info("Creating settings UI . . .");
    // Make a VerticalLayoutGroup to ensure that the score submission text is above the setting.
    VerticalLayoutGroup* layout = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(get_rectTransform());
    // Make the settings aligned at the top
    layout->set_childAlignment(UnityEngine::TextAnchor::UpperCenter);
    layout->set_childForceExpandHeight(false);
    layout->set_childControlHeight(true);

    // Create some text to show whether or not score submission will be disabled
    TextMeshProUGUI* scoreSubmissionText = BeatSaberUI::CreateText(layout->get_rectTransform(), "");
    setScoreSubmissionText(scoreSubmissionText);
    
    // Make an action for whenever the speed is changed
    UnityAction_1<float>* onSettingChange = il2cpp_utils::MakeDelegate<UnityAction_1<float>*>(
        classof(UnityAction_1<float>*), scoreSubmissionText, onResumeAnimationSpeedSettingChange
    );
    // Make a setting for changing the speed in UI
    BeatSaberUI::CreateIncrementSetting(layout->get_rectTransform(), "Resume animation speed",
                                        1, 0.1, getConfig().config["speed"].GetFloat(), // Use the current speed value in the config
                                        onSettingChange);
}

// Save the config upon leaving the settings menu
void SettingsViewController::DidDeactivate(bool removedFromHierarchy, bool systemScreenDisabling) {
    getLogger().info("Saving config on settings menu exit.");
    getConfig().Write();
}