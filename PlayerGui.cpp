

#include "PlayerGui.h"


void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);
}
if (isRepeating && transportSource.getCurrentPosition() >= transportSource.getLengthInSeconds())
 {
     transportSource.setPosition(0.0);
     transportSource.start();
 }
 get next AudioBlack

void PlayerGUI::releaseResources()
{
    playerAudio.releaseResources();
}
void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}




PlayerGUI::PlayerGUI()
{

    // Add buttons
    for (auto* btn : { &loadButton, &restartButton , &stopButton , &gotostartButton , &muteButton,&repeatButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // Volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

}
void PlayerGUI::resized()
{
    int y = 20;
    loadButton.setBounds(20, y, 100, 40);
    restartButton.setBounds(140, y, 80, 40);
    stopButton.setBounds(240, y, 80, 40);
    gotostartButton.setBounds(340, y, 80, 40);
    muteButton.setBounds(440, y, 80, 40);
    repeatButton.setBounds(540, y, 80, 40);
    /*prevButton.setBounds(340, y, 80, 40);
    nextButton.setBounds(440, y, 80, 40);*/

    volumeSlider.setBounds(20, 100, getWidth() - 40, 30);
}


PlayerGUI::~PlayerGUI()
{
}
void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        juce::FileChooser chooser("Select audio files...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile())
                {
                    playerAudio.loadFile(file);
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
else    if (button == &repeatButton)
    {
        isRepeating = !isRepeating; 

        if (isRepeating)
            repeatButton.setButtonText("Repeat: On");
        else
            repeatButton.setButtonText("Repeat: Off");
    }


void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
        playerAudio.setGain((float)slider->getValue());
}




