#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    //audioDeviceManager.initialise(2, 2, nullptr, true, "Null Audio Device");
    audioDeviceManager.initialise(2, 0, nullptr, true, "Null Audio Device");
    audioDeviceManager.addAudioCallback(this);
    startTimerHz(60); // Call timerCallback 30 times per second.
    setSize (1920, 1080);
    
    auto surferBinary = BinaryData::surfer_png;
    auto surferBinarySize = BinaryData::surfer_pngSize;
    juce::MemoryInputStream stream(surferBinary, surferBinarySize, false);
    juce::Image surferFullSize = juce::ImageFileFormat::loadFrom(stream);

//    juce::Image surferFullSize = juce::ImageFileFormat::loadFrom(juce::File("surfer.png"));
    surferImage = surferFullSize.rescaled(100, 100, juce::Graphics::highResamplingQuality);

    auto* currentDevice = audioDeviceManager.getCurrentAudioDevice();
    if (currentDevice != nullptr)
    {
        // Get and log name
        auto name = currentDevice->getName();
        juce::Logger::writeToLog("Device Name: " + name);

        // Get and log sample rate
        auto sampleRate = currentDevice->getCurrentSampleRate();
        juce::Logger::writeToLog("Sample Rate: " + juce::String(sampleRate));

        // Get and log buffer size
        auto bufferSize = currentDevice->getCurrentBufferSizeSamples();
        juce::Logger::writeToLog("Buffer Size: " + juce::String(bufferSize));

        // Get and log bit depth
        auto bitDepth = currentDevice->getCurrentBitDepth();
        juce::Logger::writeToLog("Bit Depth: " + juce::String(bitDepth));

        // You can also log other information as needed
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
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.fillAll(juce::Colours::black);

    g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::white);
    //g.drawText ("Hello World!", getLocalBounds(), juce::Justification::centred, true);
    

    //Basic fill
//    int widthToFill = static_cast<int>(static_cast<float>(width) * smoothedRms[0]);
//    g.setColour(juce::Colours::red);
//    g.fillRect(0, 0, widthToFill,
    
    //Basic wave
//    juce::Path rmsPath;
//    for (int i = 0; i < rmsHistory.size(); ++i)
//    {
//        float x = (float)i / (float)maxHistory * (float)width;
//        float y = juce::jmap(rmsHistory[i], 0.0f, 1.0f, (float)height, 0.0f);
//
//        if (i == 0)
//        {
//            rmsPath.startNewSubPath(x, y);
//        }
//        else
//        {
//            rmsPath.lineTo(x, y);
//        }
//    }
//
//    g.setColour(juce::Colours::red);
//    g.strokePath(rmsPath, juce::PathStrokeType(2.0f));
    
    //Advanced wave
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
    
    // Extend path to bottom-right corner
    float lastX = (float) rmsHistory.size() / (float) maxHistory * (float) width;
    rmsPath.lineTo(lastX, height);
    
    // Draw line to bottom-left corner
    rmsPath.lineTo(0, height);
    
    // Close the path
    rmsPath.closeSubPath();
    
    // Fill the path
    g.setColour(juce::Colours::purple.withAlpha(0.3f));  // With some transparency
    g.fillPath(rmsPath);

    // Optionally, stroke the original line curve on top
    g.setColour(juce::Colours::purple);
    g.strokePath(rmsPath, juce::PathStrokeType(2.0f));
    
    //draw surfer halfway through line
    int halfWidth = static_cast<int>((float)rmsHistory.size()/2 / (float)maxHistory * (float)width);
    int halfHeight = juce::jmap(rmsHistory[rmsHistory.size()/2]*10, 0.0f, 1.0f, (float)height, 0.0f) - 80;
    
    if (!surferImage.isNull()) {
        g.drawImageAt(surferImage, halfWidth, halfHeight);
    }
    

    

}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
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
                // Update the RMS history
                if (rmsHistory.size() >= maxHistory)
                {
                    rmsHistory.pop_front();
                }
                rmsHistory.push_back(smoothedRms[0]);
            }


            
        }
    }
}


void MainComponent::timerCallback() {
    repaint();
}
