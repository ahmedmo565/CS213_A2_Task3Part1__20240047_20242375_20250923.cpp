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
    auto theme = playerAudio.getCurrentTheme();
    g.fillAll(theme.backgroundColour);
}

PlayerGUI::PlayerGUI() : playlistModel(*this), playerAudio(), waveForm(playerAudio)
{
    for (auto* btn : { &loadButton, &restartButton, &stopButton, &gotostartButton,
                      &muteButton, &repeatButton, &prevButton, &nextButton, &clearPlaylistButton,
                      &aButton, &bButton, &clearabButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    speedSlider.setRange(0.1, 2.0, 0.01);
    speedSlider.setValue(1.0);
    speedSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    speedSlider.setTextValueSuffix("x");
    speedSlider.addListener(this);
    addAndMakeVisible(speedSlider);

    positionSlider.setRange(0.0, 1.0, 0.001);
    positionSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    positionSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    positionSlider.addListener(this);
    addAndMakeVisible(positionSlider);

    timeLabel.setText("0:00,0:00", juce::dontSendNotification);
    addAndMakeVisible(timeLabel);

    addAndMakeVisible(waveForm);

    metadataLabel.setText("File Information:", juce::dontSendNotification);
    addAndMakeVisible(metadataLabel);

    metadataDisplay.setMultiLine(true);
    metadataDisplay.setReadOnly(true);
    metadataDisplay.setCaretVisible(false);
    metadataDisplay.setText("No file loaded");
    addAndMakeVisible(metadataDisplay);

    playlistLabel.setText("Playlist:", juce::dontSendNotification);
    addAndMakeVisible(playlistLabel);

    playlistBox.setModel(&playlistModel);
    addAndMakeVisible(playlistBox);

    Theme yotsubaTheme;
    yotsubaTheme.backgroundColour = juce::Colour(0xfffffaf0);
    yotsubaTheme.buttonColour = juce::Colour(0xff8fa876);
    yotsubaTheme.buttonTextColour = juce::Colour(0xff000000);
    yotsubaTheme.sliderColour = juce::Colour(0xffe8d8c8);
    yotsubaTheme.sliderThumbColour = juce::Colour(0xff8fa876);
    yotsubaTheme.labelColour = juce::Colour(0xff000000);
    yotsubaTheme.waveformColour = juce::Colour(0xff8fa876);
    yotsubaTheme.playlistBackground = juce::Colour(0xfff5f0e8);
    yotsubaTheme.playlistText = juce::Colour(0xff000000);
    playerAudio.setTheme(yotsubaTheme);

    applyTheme();
    startTimer(100);
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

    aButton.setBounds(20, 200, 80, 40);
    bButton.setBounds(120, 200, 80, 40);
    clearabButton.setBounds(220, 200, 100, 40);

    volumeSlider.setBounds(20, 260, getWidth() - 40, 30);
    speedSlider.setBounds(20, 300, getWidth() - 40, 30);

    positionSlider.setBounds(20, 340, getWidth() - 40, 20);
    timeLabel.setBounds(getWidth() - 100, 320, 80, 20);
    waveForm.setBounds(20, 370, getWidth() - 40, 100);

    int playlistWidth = getWidth() / 2 - 30;
    int metadataWidth = getWidth() - playlistWidth - 60;

    metadataLabel.setBounds(20, 460, metadataWidth, 25);
    metadataDisplay.setBounds(20, 485, metadataWidth, 200);

    playlistLabel.setBounds(metadataWidth + 40, 460, playlistWidth, 25);
    playlistBox.setBounds(metadataWidth + 40, 485, playlistWidth, 200);
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
            muteButton.setButtonText("Unmute");
        else
            muteButton.setButtonText("Mute");
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

    if (button == &aButton)
    {
        abStart = playerAudio.getPosition();
    }

    if (button == &bButton)
    {
        abEnd = playerAudio.getPosition();
        playerAudio.setabLoop(abStart, abEnd);
    }

    if (button == &clearabButton)
    {
        playerAudio.clearabLoop();
    }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
        playerAudio.setGain((float)slider->getValue());
    if (slider == &speedSlider)
        playerAudio.setSpeed((float)slider->getValue());

    if (slider == &positionSlider)
    {
        double newPosition = positionSlider.getValue() * playerAudio.getLength();
        playerAudio.setPosition(newPosition);
    }
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

void PlayerGUI::timerCallback()
{
    if (playerAudio.getLength() > 0)
    {
        double progress = playerAudio.getPosition() / playerAudio.getLength();
        positionSlider.setValue(progress, juce::dontSendNotification);

        juce::String timeText = formatTime(playerAudio.getPosition()) + " / " + formatTime(playerAudio.getLength());
        timeLabel.setText(timeText, juce::dontSendNotification);
    }

    waveForm.repaint();
}

juce::String PlayerGUI::formatTime(double seconds)
{
    if (seconds < 0) seconds = 0;

    int totalSeconds = (int)seconds;
    int mins = totalSeconds / 60;
    int secs = totalSeconds % 60;

    return juce::String::formatted("%d:%02d", mins, secs);
}

void PlayerGUI::applyTheme()
{
    auto theme = playerAudio.getCurrentTheme();

    for (auto* btn : { &loadButton, &restartButton, &stopButton, &gotostartButton,
                      &muteButton, &repeatButton, &prevButton, &nextButton, &clearPlaylistButton,
                      &aButton, &bButton, &clearabButton })
    {
        btn->setColour(juce::TextButton::buttonColourId, theme.buttonColour);
        btn->setColour(juce::TextButton::textColourOnId, theme.buttonTextColour);
        btn->setColour(juce::TextButton::textColourOffId, theme.buttonTextColour);
    }

    volumeSlider.setColour(juce::Slider::backgroundColourId, theme.sliderColour);
    volumeSlider.setColour(juce::Slider::thumbColourId, theme.sliderThumbColour);
    volumeSlider.setColour(juce::Slider::trackColourId, theme.sliderColour);

    speedSlider.setColour(juce::Slider::backgroundColourId, theme.sliderColour);
    speedSlider.setColour(juce::Slider::thumbColourId, theme.sliderThumbColour);
    speedSlider.setColour(juce::Slider::trackColourId, theme.sliderColour);

    positionSlider.setColour(juce::Slider::backgroundColourId, theme.sliderColour);
    positionSlider.setColour(juce::Slider::thumbColourId, theme.sliderThumbColour);
    positionSlider.setColour(juce::Slider::trackColourId, theme.sliderColour);

    timeLabel.setColour(juce::Label::textColourId, theme.labelColour);
    metadataLabel.setColour(juce::Label::textColourId, theme.labelColour);
    playlistLabel.setColour(juce::Label::textColourId, theme.labelColour);

    metadataDisplay.setColour(juce::TextEditor::backgroundColourId, theme.backgroundColour.darker(0.2f));
    metadataDisplay.setColour(juce::TextEditor::textColourId, theme.labelColour);
    metadataDisplay.setColour(juce::TextEditor::outlineColourId, theme.sliderColour);

    playlistBox.setColour(juce::ListBox::backgroundColourId, theme.playlistBackground);

    repaint();
}
