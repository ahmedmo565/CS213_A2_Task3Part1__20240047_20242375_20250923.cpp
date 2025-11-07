#include "PlayerGui.h"

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

    Theme darkTheme;
    darkTheme.backgroundColour = juce::Colour(0xff1e1e1e);
    darkTheme.buttonColour = juce::Colour(0xff3c3c3c);
    darkTheme.buttonTextColour = juce::Colour(0xffffffff);
    darkTheme.sliderColour = juce::Colour(0xff4a4a4a);
    darkTheme.sliderThumbColour = juce::Colour(0xff007acc);
    darkTheme.labelColour = juce::Colour(0xffffffff);
    darkTheme.waveformColour = juce::Colour(0xff007acc);
    darkTheme.playlistBackground = juce::Colour(0xff252526);
    darkTheme.playlistText = juce::Colour(0xffffffff);
    playerAudio.setTheme(darkTheme);

    applyTheme();
    startTimer(100);
}

PlayerGUI::~PlayerGUI()
{
}

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

void PlayerGUI::resized()
{
    int buttonWidth = 100;
    int buttonHeight = 30;
    int margin = 10;

    int centerX = getWidth() / 2;

    loadButton.setBounds(centerX - buttonWidth / 2, 20, buttonWidth, buttonHeight);

    int secondRowY = 70;
    int secondRowCenterX = centerX - (buttonWidth * 3 + margin * 2) / 2;

    restartButton.setBounds(secondRowCenterX, secondRowY, buttonWidth, buttonHeight);
    stopButton.setBounds(secondRowCenterX + buttonWidth + margin, secondRowY, buttonWidth, buttonHeight);
    gotostartButton.setBounds(secondRowCenterX + (buttonWidth + margin) * 2, secondRowY, buttonWidth, buttonHeight);

    int thirdRowY = 110;
    int thirdRowCenterX = centerX - (buttonWidth * 3 + margin * 2) / 2;

    muteButton.setBounds(thirdRowCenterX, thirdRowY, buttonWidth, buttonHeight);
    repeatButton.setBounds(thirdRowCenterX + buttonWidth + margin, thirdRowY, buttonWidth, buttonHeight);
    clearPlaylistButton.setBounds(thirdRowCenterX + (buttonWidth + margin) * 2, thirdRowY, buttonWidth, buttonHeight);

    int fourthRowY = 150;

    aButton.setBounds(20, fourthRowY, buttonWidth, buttonHeight);
    bButton.setBounds(140, fourthRowY, buttonWidth, buttonHeight);
    clearabButton.setBounds(260, fourthRowY, buttonWidth, buttonHeight);

    prevButton.setBounds(getWidth() - 240, fourthRowY, buttonWidth, buttonHeight);
    nextButton.setBounds(getWidth() - 120, fourthRowY, buttonWidth, buttonHeight);

    volumeSlider.setBounds(20, 200, getWidth() - 40, 30);
    speedSlider.setBounds(20, 240, getWidth() - 40, 30);

    positionSlider.setBounds(20, 280, getWidth() - 40, 20);
    timeLabel.setBounds(getWidth() - 100, 260, 80, 20);
    waveForm.setBounds(20, 310, getWidth() - 40, 100);

    int playlistWidth = getWidth() / 2 - 30;
    int metadataWidth = getWidth() - playlistWidth - 60;

    metadataLabel.setBounds(20, 420, metadataWidth, 25);
    metadataDisplay.setBounds(20, 445, metadataWidth, 200);

    playlistLabel.setBounds(metadataWidth + 40, 420, playlistWidth, 25);
    playlistBox.setBounds(metadataWidth + 40, 445, playlistWidth, 200);
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
            repeatButton.setButtonText("Repeat: On");
        else
            repeatButton.setButtonText("Repeat: Off");
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

    metadataDisplay.setColour(juce::TextEditor::backgroundColourId, theme.backgroundColour.darker());
    metadataDisplay.setColour(juce::TextEditor::textColourId, theme.labelColour);
    metadataDisplay.setColour(juce::TextEditor::outlineColourId, theme.sliderColour);

    playlistBox.setColour(juce::ListBox::backgroundColourId, theme.playlistBackground);

    repaint();
}
