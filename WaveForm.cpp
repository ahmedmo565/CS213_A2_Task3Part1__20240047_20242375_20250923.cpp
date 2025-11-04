#include "WaveForm.h"

WaveForm::WaveForm(PlayerAudio& playerAudioRef)
    : playerAudio(playerAudioRef)
{
    startTimerHz(30);
}

WaveForm::~WaveForm()
{
    stopTimer();
}

void WaveForm::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::cornflowerblue);

    auto& thumbnail = playerAudio.getAudioThumbnail();

    thumbnail.drawChannels(g,
        getLocalBounds(),
        0.0,
        thumbnail.getTotalLength(),
        1.0f);

    double currentPos = playerAudio.getPosition();
    double totalLength = playerAudio.getLength();

    if (totalLength > 0)
    {
        float lineX = (float)(currentPos / totalLength) * (float)getWidth();

        g.setColour(juce::Colours::white);
        g.drawVerticalLine((int)lineX, 0.0f, (float)getHeight());
    }
    auto currentSeconds = playerAudio.getPosition();
    auto totalSeconds = thumbnail.getTotalLength();

    int totalMinutes = (int)totalSeconds / 60;
    int totalSecondsRemaining = (int)totalSeconds % 60;
    juce::String totalTimeString = juce::String(totalMinutes) + " : " + juce::String(totalSecondsRemaining).paddedLeft('0', 2);

    int currentMinutes = (int)currentSeconds / 60;
    int currentSecondsRemaining = (int)currentSeconds % 60;
    juce::String currentTimeString = juce::String(currentMinutes) + " : " + juce::String(currentSecondsRemaining).paddedLeft('0', 2);

    juce::String timeString = currentTimeString + " / " + totalTimeString;

    g.setColour(juce::Colours::white);
    g.setFont(16.0f);
    auto textBounds = getLocalBounds().removeFromBottom(10);
    g.drawText(timeString, textBounds, juce::Justification::centredRight, true);
}

void WaveForm::timerCallback()
{
    repaint();
}