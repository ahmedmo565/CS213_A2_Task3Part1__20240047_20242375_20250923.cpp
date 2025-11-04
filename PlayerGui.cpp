#include "playerGui.h"

void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);
}

void PlayerGUI::releaseResources()
{
    playerAudio.releaseResources();
}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

PlayerGUI::PlayerGUI() : playlistModel(*this),playerAudio(), waveForm(playerAudio)
{
    
    for (auto* btn : { &loadButton, &restartButton, &stopButton, &gotostartButton,
                      &muteButton, &repeatButton, &prevButton, &nextButton, &clearPlaylistButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

  
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);
    
    // Speed slider
    speedSlider.setRange(0.1, 2.0, 0.01);
    speedSlider.setValue(1.0);
    speedSlider.setTextValueSuffix("x");
    speedSlider.addListener(this);
    addAndMakeVisible(speedSlider);

    addAndMakeVisible(waveForm);
    
    metadataLabel.setText("File Information:", juce::dontSendNotification);
    metadataLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(metadataLabel);

    metadataDisplay.setMultiLine(true);
    metadataDisplay.setReadOnly(true);
    metadataDisplay.setCaretVisible(false);
    metadataDisplay.setColour(juce::TextEditor::backgroundColourId, juce::Colours::darkgrey.darker());
    metadataDisplay.setColour(juce::TextEditor::textColourId, juce::Colours::white);
    metadataDisplay.setColour(juce::TextEditor::outlineColourId, juce::Colours::grey);
    metadataDisplay.setText("No file loaded");
    addAndMakeVisible(metadataDisplay);

  
    playlistLabel.setText("Playlist:", juce::dontSendNotification);
    playlistLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(playlistLabel);

    playlistBox.setModel(&playlistModel);
    playlistBox.setColour(juce::ListBox::backgroundColourId, juce::Colours::white);
    addAndMakeVisible(playlistBox);

    addAndMakeVisible(waveForm);
}

void PlayerGUI::resized()
{
    int y = 20;
    loadButton.setBounds(20, y, 100, 40);
    restartButton.setBounds(140, y, 80, 40);
    stopButton.setBounds(240, y, 80, 40);
    gotostartButton.setBounds(340, y, 80, 40);
    muteButton.setBounds(440, y, 80, 40);
    repeatButton.setBounds(560, y, 80, 40);
    prevButton.setBounds(660, y, 80, 40);
    nextButton.setBounds(760, y, 80, 40);
    clearPlaylistButton.setBounds(860, y, 100, 40);

    volumeSlider.setBounds(20, 80, getWidth() - 40, 30);
    speedSlider.setBounds(20, 120, getWidth() - 40, 30);
    
    waveForm.setBounds(20, 160, getWidth() - 40, 100);
    
    int playlistWidth = getWidth() / 2 - 30;
    int metadataWidth = getWidth() - playlistWidth - 60;

    metadataLabel.setBounds(20, 280, metadataWidth, 25);
    metadataDisplay.setBounds(20, 305, metadataWidth, 200);

    playlistLabel.setBounds(metadataWidth + 40, 280, playlistWidth, 25);
    playlistBox.setBounds(metadataWidth + 40, 305, playlistWidth, 200);
}
 

PlayerGUI::~PlayerGUI()
{
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "select audio files...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectMultipleItems,
            [this](const juce::FileChooser& fc)
            {
                auto results = fc.getResults();
                for (auto& file : results)
                {
                    if (file.existsAsFile())
                    {
                        playerAudio.addToPlaylist(file);
                    }
                }
                updatePlaylist();

              
                if (playerAudio.getPlaylistSize() > 0 && playerAudio.getCurrentPlaylistIndex() == -1)
                {
                    playerAudio.playFileAtIndex(0);
                    updateMetadataDisplay();
                }
            });
    }

    if (button == &restartButton)
    {
        playerAudio.start();
    }

    if (button == &stopButton)
    {
        playerAudio.stop();
        playerAudio.setPosition(0.0);
    }

    if (button == &gotostartButton)
    {
        playerAudio.setPosition(0.0);
    }

    if (button == &muteButton)
    {
        playerAudio.mute();
        if (playerAudio.isMuted())
        {
            muteButton.setButtonText("Unmute");
        }
        else
        {
            muteButton.setButtonText("Mute");
        }
    }

    if (button == &repeatButton)
    {
        playerAudio.toggleRepeat();
        isRepeating = playerAudio.isRepeatEnabled();

        if (isRepeating)
            repeatButton.setButtonText("repeat: on");
        else
            repeatButton.setButtonText("repeat: off");
    }

    
    if (button == &prevButton)
    {
        playerAudio.playPrevious();
        updateMetadataDisplay();
        updatePlaylist();
    }

    if (button == &nextButton)
    {
        playerAudio.playNext();
        updateMetadataDisplay();
        updatePlaylist();
    }

    if (button == &clearPlaylistButton)
    {
        playerAudio.clearPlaylist();
        updatePlaylist();
        metadataDisplay.setText("No file loaded");
    }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
        playerAudio.setGain((float)slider->getValue());
    if (slider == &speedSlider)
        playerAudio.setSpeed((float)slider->getValue());
}

void PlayerGUI::updateMetadataDisplay()
{
    juce::StringArray metadata = playerAudio.getMetadata();
    juce::String displayText;

    if (metadata.size() > 0)
    {
        for (const auto& line : metadata)
        {
            displayText += line + "\n";
        }

        
        int currentIndex = playerAudio.getCurrentPlaylistIndex();
        int totalFiles = playerAudio.getPlaylistSize();
        if (currentIndex >= 0 && totalFiles > 0)
        {
            displayText += "\nPlaylist: " + juce::String(currentIndex + 1) + " / " + juce::String(totalFiles);
        }
    }
    else
    {
        displayText = "No metadata available\nFile: " + playerAudio.getCurrentFileName();
    }

    metadataDisplay.setText(displayText);
}

void PlayerGUI::updatePlaylist()
{
    playlistBox.updateContent();
    playlistBox.repaint();
}





