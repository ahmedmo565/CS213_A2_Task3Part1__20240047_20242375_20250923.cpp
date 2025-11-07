##pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"
#include "WaveForm.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer
{
public:
    PlayerGUI();
    ~PlayerGUI() override;

    void resized() override;
    void paint(juce::Graphics& g) override;
    void timerCallback() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    void applyTheme();

private:
    PlayerAudio playerAudio;

    juce::TextButton loadButton{ "Load Files" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton gotostartButton{ "Start" };
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton repeatButton{ "Repeat: Off" };
    juce::TextButton aButton{ "Set A" };
    juce::TextButton bButton{ "Set B" };
    juce::TextButton clearabButton{ "Clear A&B" };
    juce::TextButton prevButton{ "Previous" };
    juce::TextButton nextButton{ "Next" };
    juce::TextButton clearPlaylistButton{ "Clear Playlist" };

    juce::Slider volumeSlider;
    juce::Slider speedSlider;
    juce::Slider positionSlider;
    juce::Label timeLabel;

    juce::Label metadataLabel;
    juce::TextEditor metadataDisplay;
    juce::Label playlistLabel;
    juce::ListBox playlistBox;

    juce::String formatTime(double seconds);

    std::unique_ptr<juce::FileChooser> fileChooser;
    bool isRepeating = false;
    double abStart = 0.0;
    double abEnd = 0.0;

    WaveForm waveForm;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void updateMetadataDisplay();
    void updatePlaylist();

    class PlaylistModel : public juce::ListBoxModel
    {
    public:
        PlaylistModel(PlayerGUI& owner) : owner(owner) {}

        int getNumRows() override
        {
            return owner.playerAudio.getPlaylistSize();
        }

        void paintListBoxItem(int rowNumber, juce::Graphics& g,
            int width, int height, bool rowIsSelected) override
        {
            auto theme = owner.playerAudio.getCurrentTheme();
            if (rowIsSelected)
                g.fillAll(theme.buttonColour);
            else
                g.fillAll(theme.playlistBackground);

            g.setColour(theme.playlistText);
            g.setFont(14.0f);

            juce::String itemText = owner.playerAudio.getPlaylistItemName(rowNumber);
            if (rowNumber == owner.playerAudio.getCurrentPlaylistIndex())
                itemText = "> " + itemText;

            g.drawText(itemText, 4, 0, width - 4, height, juce::Justification::centredLeft);
        }

        void listBoxItemClicked(int row, const juce::MouseEvent&) override
        {
            owner.playerAudio.playFileAtIndex(row);
            owner.updateMetadataDisplay();
            owner.updatePlaylist();
        }

        void deleteKeyPressed(int row) override
        {
            owner.playerAudio.removePlaylistItem(row);
            owner.updatePlaylist();
        }

    private:
        PlayerGUI& owner;
    };

    PlaylistModel playlistModel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};
