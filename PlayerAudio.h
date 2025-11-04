#pragma once
#include <JuceHeader.h>

class PlayerAudio
{
public:
    PlayerAudio();
    ~PlayerAudio();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    bool loadFile(const juce::File& file);
    void play();
    void stop();
    void start();
    void mute();
    bool isMuted() const;
    void setGain(float gain);
    void setPosition(double pos);
    double getPosition() const;
    double getLength() const;
    void toggleRepeat();
    void setRepeat(bool shouldRepeat);
    bool isRepeatEnabled() const;

    juce::String getCurrentFileName() const;
    juce::StringArray getMetadata() const;

    void addToPlaylist(const juce::File& file);
    void clearPlaylist();
    void playNext();
    void playPrevious();
    void playFileAtIndex(int index);
    int getCurrentPlaylistIndex() const;
    int getPlaylistSize() const;
    juce::String getPlaylistItemName(int index) const;
    juce::File getPlaylistFile(int index) const;
    void removePlaylistItem(int index);
    void setSpeed(double s);
    juce::AudioThumbnail& getAudioThumbnail();

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;

    
    juce::File currentFile;
    juce::StringArray metadata;

    juce::Array<juce::File> playlist;
    int currentPlaylistIndex = -1;

    bool muted = false;
    float previousVolume = 1.0f;
    bool isRepeating = false;

    std::unique_ptr<juce::ResamplingAudioSource>resampleSource;
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail audioThumbnail;

    void extractMetadata(juce::AudioFormatReader* reader, const juce::File& file);
    juce::String formatTime(double seconds);
    bool loadFileInternal(const juce::File& file);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};

