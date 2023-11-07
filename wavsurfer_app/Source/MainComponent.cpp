#include "MainComponent.h"
#include <chrono>

//==============================================================================
MainComponent::MainComponent()
{
     
    // fill in fft windowing function
    juce::dsp::WindowingFunction<float>::fillWindowingTables(windowingFunction.data(), fftSize, juce::dsp::WindowingFunction<float>::hann);
    
    // manually (for now) computing low, mid, and high frequency bands
    // TODO: Define freq range and set these based off range/numbands?
    bandStartBin = std::vector<int>(numBands, 0);
    bandEndBin = std::vector<int>(numBands, 0);
    bandStartBin[0] = static_cast<int>(std::ceil(lowFreq / frequencyResolution));
    bandEndBin[0] = static_cast<int>(std::floor(midFreq / frequencyResolution));
    bandStartBin[1] = bandEndBin[0];
    bandEndBin[1] = static_cast<int>(std::floor(highFreq / frequencyResolution));
    bandStartBin[2] = bandEndBin[1];
    bandEndBin[2] = fftSize / 2; // nyquist limit

    // setting up dqVecs: a vector containing a vector per channel which contains a deque per frequency band
    for (int i = 0; i < supportedChannels; ++i)
    {
        dqVecs.push_back(std::vector<std::deque<float>>());
        // set up second level (deque at each vector component)
        for (int j = 0; j < numBands; ++j)
        {
            dqVecs[i].push_back(std::deque<float>());
        }
    }

    // audio device setup
    audioDeviceManager.initialise(2, 0, nullptr, true, "Null Audio Device");
    audioDeviceManager.addAudioCallback(this);
    
    // page size setup
    setSize (1920, 1080);
    
    // repaint callback setup
    startTimerHz(60);
    
    // TODO: Add back in surfer image setup if decide to add image back in
    //    auto surferBinary = BinaryData::surfer_png;
    //    auto surferBinarySize = BinaryData::surfer_pngSize;
    //    juce::MemoryInputStream stream(surferBinary, surferBinarySize, false);
    //    juce::Image surferFullSize = juce::ImageFileFormat::loadFrom(stream);
    
    
    // Set up background image (optional)
    /*
    auto backgroundBinary = BinaryData::IMG_9897_jpeg;
    auto backgroundBinarySize = BinaryData::IMG_9897_jpegSize;
    juce::MemoryInputStream stream(backgroundBinary, backgroundBinarySize, false);
    juce::Image backgroundImageRaw = juce::ImageFileFormat::loadFrom(stream);
    backgroundImage = backgroundImageRaw.rescaled(1920, 1080, juce::Graphics::highResamplingQuality);
    */
}


MainComponent::~MainComponent()
{
    audioDeviceManager.removeAudioCallback(this);
}


//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    int width = getWidth();
    int height = getHeight();
    g.fillAll(juce::Colours::black);
    g.drawImage (backgroundImage, getLocalBounds ().toFloat ());

    float heightMultiplier = 1;

    // Paint every channel's frequency bins
    for (int i = 0; i < supportedChannels; ++i) // channels
    {
        for (int j = 0; j < numBands; ++j) // freq bands
        {
            // rough height multiplier to account of low freq rms being disproportionately larger
            heightMultiplier = 3 * pow(5.0 / 4.0, j) / (5 * numBands);
            
            juce::Path rmsPath;
            for (int k = 0; k < dqVecs[i][j].size(); ++k) // rms history deque elements
            {
                float x = (float)k / (float)maxHistory * (float)width;
                float y = juce::jmap(dqVecs[i][j][k]*heightMultiplier, 0.0f, 1.0f, (float)height, 0.0f);
                if (k == 0)
                {
                    rmsPath.startNewSubPath(x, y);
                }
                else
                {
                    rmsPath.lineTo(x, y);
                }
            }

            // extend path to bottom-right corner
            float lastX = (float) dqVecs[i][j].size() / (float) maxHistory * (float) width;
            rmsPath.lineTo(lastX, height);

            // draw line to bottom-left corner
            rmsPath.lineTo(0, height);
            rmsPath.closeSubPath();
            
            g.setColour(juce::Colours::purple.withAlpha(0.3f));
            g.fillPath(rmsPath);
            g.setColour(juce::Colours::purple);
            g.strokePath(rmsPath, juce::PathStrokeType(2.0f));

        }
    }

    // TODO: Decide whether to add back in surfer (below)
    /*
     if (!surferImage.isNull()) {
        g.drawImageAt(surferImage, halfWidth, halfHeight);
     */
}


void MainComponent::resized()
{
    // called when main component resized
}


// at 512 samples per second and 44.1khz sample rate, callback should be called ~86 times/sec, giving this function a theoretical upper limit of 11.63ms to maintain real time constraints.
// currently runs at ~.05ms, so there is definitely room for more processing
void MainComponent::audioDeviceIOCallbackWithContext (const float *const *inputChannelData, int numInputChannels, float *const *outputChannelData, int numOutputChannels, int numSamples, const juce::AudioIODeviceCallbackContext &context)
{
    auto start = std::chrono::high_resolution_clock::now();
    callbackCount++;
    
    float fftData[2 * fftSize] = {0};

    for (int inputChannel = 0; inputChannel < numInputChannels; ++inputChannel)
    {
        if(inputChannelData[inputChannel] == nullptr)
        {
            continue;
        }

        // windowing
        for(int i = 0; i < fftSize; ++i)
        {
            fftData[i] = inputChannelData[inputChannel][i] * windowingFunction[i];
        }
        fft->performFrequencyOnlyForwardTransform(fftData);

        // calculating rms for each freq band
        for(int band = 0; band < numBands; ++band)
        {
            float sumOfSquares = 0.0f;
            int startBin = bandStartBin[band];
            int endBin = bandEndBin[band];

            for(int bin = startBin; bin < endBin; ++bin)
            {
                float magnitude = fftData[bin];
                sumOfSquares += magnitude * magnitude;
            }

            if (inputChannel < supportedChannels)
            {
                if (dqVecs[inputChannel][band].size() >= maxHistory)
                {
                    dqVecs[inputChannel][band].pop_front();
                }
                
                // Exponential smoothing: newSmoothedValue = ((1 - smoothingFactor) * oldSmoothedValue) + (smoothingFactor * newRawValue)
                auto dqSize = dqVecs[inputChannel][band].size();
                if (dqSize == 0)
                {
                    auto smoothedSample = std::sqrt(sumOfSquares / (endBin - startBin));
                    dqVecs[inputChannel][band].push_back(smoothedSample);
                }
                else
                {
                    auto smoothedSample = ((1 - smoothingFactor) * dqVecs[inputChannel][band][dqSize - 1]) + (smoothingFactor * std::sqrt(sumOfSquares / (endBin - startBin)));
                    dqVecs[inputChannel][band].push_back(smoothedSample);
                }
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms_double = end - start;
    std::cout<< "Audio callback took approx " << ms_double.count() << "ms\n";

}


void MainComponent::timerCallback() {
    // time calculations on callback
    timerCalls++;
    if(timerCalls == 60)
    {
        printf("Callback was called %d times in the past second\n", callbackCount);
        callbackCount = 0;
        timerCalls = 0;
    }
    
    // repainting main window
    repaint();
}


// helper fxn to pritn info about the audio device
void printAudioDeviceInfo(juce::AudioDeviceManager & audioDeviceManager) {
    auto* currentDevice = audioDeviceManager.getCurrentAudioDevice();
    if (currentDevice != nullptr)
    {
        auto name = currentDevice->getName();
        juce::Logger::writeToLog("Device Name: " + name);

        auto sampleRate = currentDevice->getCurrentSampleRate();
        juce::Logger::writeToLog("Sample Rate: " + juce::String(sampleRate));

        auto bufferSize = currentDevice->getCurrentBufferSizeSamples();
        juce::Logger::writeToLog("Buffer Size: " + juce::String(bufferSize));

        auto bitDepth = currentDevice->getCurrentBitDepth();
        juce::Logger::writeToLog("Bit Depth: " + juce::String(bitDepth));
    }
}
