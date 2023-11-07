#pragma once

#include <JuceHeader.h>
#include <vector>
#include <juce_dsp/juce_dsp.h>

//==============================================================================
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
    juce::AudioDeviceManager audioDeviceManager;

    int supportedChannels = 2;
    static constexpr int numBands = 3;
    const int sampleRate = 44100;
    static constexpr int fftSize = 512; // equal to numsamples

    // FFT related components
    std::array<float, fftSize> windowingFunction;
    int order = static_cast<int>(std::log2(fftSize));
    std::unique_ptr<juce::dsp::FFT> fft = std::make_unique<juce::dsp::FFT>(order);
    std::vector<int> bandStartBin;
    std::vector<int> bandEndBin;
    const float frequencyResolution = static_cast<float>(sampleRate) / fftSize;
    // TODO: set freq range and calculate these based off range/numbands instead
    const float lowFreq = 20.0f;
    const float midFreq = 250.0f;
    const float highFreq = 4000.0f;
    const float nyquist = sampleRate / 2.0f;
    
    // used for exponential smoothing of displayed signal
    const float smoothingFactor = .15;
    
    // vector containing vector per channel containing deque per freq band to store RMS for painting
    std::vector<std::vector<std::deque<float>>> dqVecs;
    const int maxHistory = 500; // max elements in each deque
    
    
    // for verifying audiocallback calls per second ~= 44100/512
    int callbackCount = 0;
    int timerCalls = 0;
    
    
    // image related
    juce::Image surferImage;
    juce::Image backgroundImage;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
