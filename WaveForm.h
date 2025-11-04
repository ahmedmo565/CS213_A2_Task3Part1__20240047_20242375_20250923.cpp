#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"
class WaveForm : public juce::Component,
    public juce::Timer
{
public:
    WaveForm(PlayerAudio& playerAudioRef);
    ~WaveForm() override;

    void paint(juce::Graphics& g) override;
    void timerCallback() override;

private:
    PlayerAudio& playerAudio;
};
