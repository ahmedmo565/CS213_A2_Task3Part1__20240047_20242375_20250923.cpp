#include"PlayerAudio.h"

PlayerAudio::PlayerAudio()
    :thumbnailCache(5), audioThumbnail(512, formatManager, thumbnailCache)
{
    formatManager.registerBasicFormats();
}

PlayerAudio::~PlayerAudio()
{
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    resampleSource = std::make_unique<juce::ResamplingAudioSource>(&transportSource, false);
    resampleSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (resampleSource)
    {
        resampleSource->getNextAudioBlock(bufferToFill);
    }
    else
    {
        transportSource.getNextAudioBlock(bufferToFill);
    }

    if (isRepeating && transportSource.getCurrentPosition() >= transportSource.getLengthInSeconds() - 0.1)
    {
        transportSource.setPosition(0.0);
    }

    if (abLoopEnabled && transportSource.getCurrentPosition() >= abEnd - 0.05)
    {
        transportSource.setPosition(abStart);
    }

    if (!playlist.isEmpty() &&
        transportSource.getCurrentPosition() >= transportSource.getLengthInSeconds() - 0.1 &&
        !transportSource.isPlaying())
    {
        playNext();
    }
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
    resampleSource->releaseResources();
}

bool PlayerAudio::loadFile(const juce::File& file)
{
    if (file.existsAsFile())
    {
        addToPlaylist(file);
        playFileAtIndex(playlist.size() - 1);
        return true;
    }
    return false;
}

bool PlayerAudio::loadFileInternal(const juce::File& file)
{
    if (file.existsAsFile())
    {
        if (auto* reader = formatManager.createReaderFor(file))
        {
            transportSource.stop();
            transportSource.setSource(nullptr);
            readerSource.reset();

            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

            transportSource.setSource(readerSource.get(),
                0,
                nullptr,
                reader->sampleRate);
            transportSource.start();
            audioThumbnail.clear();
            audioThumbnail.setSource(new juce::FileInputSource(file));
            currentFile = file;
            extractMetadata(reader, file);

            return true;
        }
    }
    return false;
}

void PlayerAudio::extractMetadata(juce::AudioFormatReader* reader, const juce::File& file)
{
    metadata.clear();

    juce::StringPairArray metaData = reader->metadataValues;

    juce::String title = metaData.getValue("Title", "");
    juce::String artist = metaData.getValue("Artist", "");
    juce::String album = metaData.getValue("Album", "");
    juce::String year = metaData.getValue("Year", "");
    juce::String comment = metaData.getValue("Comment", "");

    if (title.isEmpty())
        title = file.getFileNameWithoutExtension();

    if (artist.isEmpty())
        artist = metaData.getValue("Author", "");
    if (artist.isEmpty())
        artist = metaData.getValue("Composer", "");
    if (artist.isEmpty())
        artist = "Unknown Artist";

    double durationInSeconds = reader->lengthInSamples / reader->sampleRate;
    juce::String duration = formatTime(durationInSeconds);

    metadata.add("Title: " + title);
    metadata.add("Artist: " + artist);

    if (!album.isEmpty())
        metadata.add("Album: " + album);

    if (!year.isEmpty())
        metadata.add("Year: " + year);

    metadata.add("Duration: " + duration);
    metadata.add("Sample Rate: " + juce::String(reader->sampleRate) + " Hz");
    metadata.add("Channels: " + juce::String(reader->numChannels));
    metadata.add("Bit Depth: " + juce::String(reader->bitsPerSample));

    if (!comment.isEmpty())
        metadata.add("Comment: " + comment);
}

juce::String PlayerAudio::formatTime(double seconds)
{
    int minutes = (int)(seconds / 60);
    int secs = (int)(seconds) % 60;
    return juce::String::formatted("%d:%02d", minutes, secs);
}

juce::String PlayerAudio::getCurrentFileName() const
{
    return currentFile.getFileName();
}

juce::StringArray PlayerAudio::getMetadata() const
{
    return metadata;
}

void PlayerAudio::addToPlaylist(const juce::File& file)
{
    if (file.existsAsFile() && (file.hasFileExtension("wav") || file.hasFileExtension("mp3")))
    {
        playlist.add(file);
    }
}

void PlayerAudio::clearPlaylist()
{
    stop();
    playlist.clear();
    currentPlaylistIndex = -1;
    metadata.clear();
}

void PlayerAudio::playNext()
{
    if (playlist.size() > 0)
    {
        int nextIndex = (currentPlaylistIndex + 1) % playlist.size();
        playFileAtIndex(nextIndex);
    }
}

void PlayerAudio::playPrevious()
{
    if (playlist.size() > 0)
    {
        int prevIndex = (currentPlaylistIndex - 1 + playlist.size()) % playlist.size();
        playFileAtIndex(prevIndex);
    }
}

void PlayerAudio::playFileAtIndex(int index)
{
    if (index >= 0 && index < playlist.size())
    {
        if (loadFileInternal(playlist[index]))
        {
            currentPlaylistIndex = index;
            start();
        }
    }
}

int PlayerAudio::getCurrentPlaylistIndex() const
{
    return currentPlaylistIndex;
}

int PlayerAudio::getPlaylistSize() const
{
    return playlist.size();
}

juce::String PlayerAudio::getPlaylistItemName(int index) const
{
    if (index >= 0 && index < playlist.size())
    {
        return playlist[index].getFileName();
    }
    return "";
}

juce::File PlayerAudio::getPlaylistFile(int index) const
{
    if (index >= 0 && index < playlist.size())
    {
        return playlist[index];
    }
    return juce::File();
}

void PlayerAudio::removePlaylistItem(int index)
{
    if (index >= 0 && index < playlist.size())
    {
        if (index == currentPlaylistIndex)
        {
            stop();
            currentPlaylistIndex = -1;
        }
        else if (index < currentPlaylistIndex)
        {
            currentPlaylistIndex--;
        }

        playlist.remove(index);
    }
}

void PlayerAudio::play()
{
    transportSource.start();
}

void PlayerAudio::stop()
{
    transportSource.stop();
}

void PlayerAudio::setGain(float gain)
{
    transportSource.setGain(gain);
}

void PlayerAudio::setPosition(double pos)
{
    transportSource.setPosition(pos);
}

double PlayerAudio::getPosition() const
{
    return transportSource.getCurrentPosition();
}

double PlayerAudio::getLength() const
{
    return transportSource.getLengthInSeconds();
}

void PlayerAudio::setSpeed(double s)
{
    if (resampleSource)
    {
        resampleSource->setResamplingRatio(s);
    }
}

void PlayerAudio::start()
{
    transportSource.start();
}

void PlayerAudio::mute()
{
    if (muted == false)
    {
        previousVolume = transportSource.getGain();
        setGain(0.0f);
        muted = true;
    }
    else
    {
        setGain(previousVolume);
        muted = false;
    }
}

bool PlayerAudio::isMuted() const
{
    return muted;
}

void PlayerAudio::toggleRepeat()
{
    isRepeating = !isRepeating;
}

void PlayerAudio::setRepeat(bool shouldRepeat)
{
    isRepeating = shouldRepeat;
}

bool PlayerAudio::isRepeatEnabled() const
{
    return isRepeating;
}

void PlayerAudio::setabLoop(double startTime, double endTime)
{
    abStart = startTime;
    abEnd = endTime;
    abLoopEnabled = true;
}

void PlayerAudio::clearabLoop()
{
    abLoopEnabled = false;
}

bool PlayerAudio::isabLoopEnabled() const
{
    return abLoopEnabled;
}

juce::AudioThumbnail& PlayerAudio::getAudioThumbnail()
{
    return audioThumbnail;
}


