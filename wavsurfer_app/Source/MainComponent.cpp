#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    audioDeviceManager.initialise(2, 0, nullptr, true, "Null Audio Device");
    audioDeviceManager.addAudioCallback(this);
    startTimerHz(60);
    setSize (1920, 1080);
    
    auto surferBinary = BinaryData::surfer_png;
    auto surferBinarySize = BinaryData::surfer_pngSize;
    juce::MemoryInputStream stream(surferBinary, surferBinarySize, false);
    juce::Image surferFullSize = juce::ImageFileFormat::loadFrom(stream);

    surferImage = surferFullSize.rescaled(100, 100, juce::Graphics::highResamplingQuality);

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

MainComponent::~MainComponent()
{
    audioDeviceManager.removeAudioCallback (this);
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    int width = getWidth();
    int height = getHeight();
    g.fillAll(juce::Colours::black);
    
    // rms path for channel 1
    juce::Path rmsPath;
    for (int i = 0; i < rmsHistory.size(); ++i)
    {
        float x = (float)i / (float)maxHistory * (float)width;
        float y = juce::jmap(rmsHistory[i]*10, 0.0f, 1.0f, (float)height, 0.0f);
        
        if (i == 0)
        {
            rmsPath.startNewSubPath(x, y);
        }
        else
        {
            rmsPath.lineTo(x, y);
        }
    }

    // extend path to bottom-right corner
    float lastX = (float) rmsHistory.size() / (float) maxHistory * (float) width;
    rmsPath.lineTo(lastX, height);
    
    // draw line to bottom-left corner
    rmsPath.lineTo(0, height);
    rmsPath.closeSubPath();
    
    // fill path
    g.setColour(juce::Colours::purple.withAlpha(0.3f));
    g.fillPath(rmsPath);
    
    // set line color
    g.setColour(juce::Colours::purple);
    g.strokePath(rmsPath, juce::PathStrokeType(2.0f));
    
    // draw surfer halfway through line
    int halfWidth = static_cast<int>((float)rmsHistory.size()/2 / (float)maxHistory * (float)width);
    int halfHeight = juce::jmap(std::max(rmsHistory[rmsHistory.size()/2], rmsHistory2[rmsHistory.size()/2])*10, 0.0f, 1.0f, (float)height, 0.0f) - 80;

    if (!surferImage.isNull()) {
        g.drawImageAt(surferImage, halfWidth, halfHeight);
    }
    
    

    // rms path for channel 2
    juce::Path rmsPath2;
    for (int i = 0; i < rmsHistory.size(); ++i)
    {
        float x = (float)i / (float)maxHistory * (float)width;
        float y = juce::jmap(rmsHistory2[i]*10, 0.0f, 1.0f, (float)height, 0.0f);
        
        if (i == 0)
        {
            rmsPath2.startNewSubPath(x, y);
        }
        else
        {
            rmsPath2.lineTo(x, y);
        }
    }

    // extend path to bottom-right corner
    rmsPath2.lineTo(lastX, height);
    
    // draw line to bottom-left corner
    rmsPath2.lineTo(0, height);
    rmsPath2.closeSubPath();
    
    // fill path
    g.setColour(juce::Colours::blue.withAlpha(0.3f));
    g.fillPath(rmsPath2);
    
    // set line color
    g.setColour(juce::Colours::blue);
    g.strokePath(rmsPath2, juce::PathStrokeType(2.0f));
    

    

}

void MainComponent::resized()
{
    // called when MC resized. update child component positions here
}

void MainComponent::audioDeviceIOCallbackWithContext (const float *const *inputChannelData, int numInputChannels, float *const *outputChannelData, int numOutputChannels, int numSamples, const juce::AudioIODeviceCallbackContext &context)
{
    for (int inputChannel = 0; inputChannel < numInputChannels; ++inputChannel)
    {
        if(inputChannelData[inputChannel]!= nullptr)
        {
            float sumOfSquares = 0.0f;
            for(int sample = 0; sample < numSamples; ++sample)
            {
                float sampleVal =inputChannelData[inputChannel][sample];
                sumOfSquares += sampleVal * sampleVal;
                //printf("Input[%d][%d] = %f\n", inputChannel, sample, inputChannelData[inputChannel][sample]);
            }
            
            rms[inputChannel]  = std::sqrt(sumOfSquares/numSamples);
            printf("RMS for channel %d is %f\n", inputChannel, rms[inputChannel]);
            smoothedRms[inputChannel] = ((1.0f - smoothingFactor) * smoothedRms[inputChannel]) + (smoothingFactor * rms[inputChannel]);
            if(inputChannel == 0)
            {
                if (rmsHistory.size() >= maxHistory)
                {
                    rmsHistory.pop_front();
                }
                if (rmsHistory2.size() >= maxHistory)
                {
                    rmsHistory2.pop_front();
                }
                rmsHistory.push_back(smoothedRms[0]);
                rmsHistory2.push_back(smoothedRms[1]);
            }


            
        }
    }
}


void MainComponent::timerCallback() {
    repaint();
}
