#pragma once

#include <JuceHeader.h>
#include <vector>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::Component, public juce::AudioIODeviceCallback, public juce::Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    // AudioIODEviceCallback necessities
    void audioDeviceIOCallbackWithContext (const float *const *inputChannelData, int numInputChannels, float *const *outputChannelData, int numOutputChannels, int numSamples, const juce::AudioIODeviceCallbackContext &context) override;
    void audioDeviceAboutToStart (juce::AudioIODevice *device) override {};
    void audioDeviceStopped() override {};
    void audioDeviceError (const juce::String &errorMessage) override {};
    void timerCallback() override;

private:
    //==============================================================================
    // Your private member variables go here...
    juce::AudioDeviceManager audioDeviceManager;
    std::vector<float> rms {0.0f, 0.0f};
    std::vector<float> smoothedRms {0.0f, 0.0f};
    //float smoothingFactor = 0.005;
    //float smoothingFactor = 0.01;
    float smoothingFactor = 0.1; //0 high smoothing, 1 low smoothing
    std::deque<float> rmsHistory;
    const int maxHistory = 100;
    juce::Image surferImage;
    
    



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
