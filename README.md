# Wavsurfer

Wavsurfer is a combination of a macOS Core Audio server plug-in that pipes input back as its output (audiosapper_plugin), and a simple JUCE app that conducts a FFT on the input signal, divides it into low/med/high frequency bins and displays the sampled RMS of each bin over time for each audio channel, in real time.

![JUCE](https://github.com/AurimasBalciunas/wavsurfer/assets/56936689/46f64eb0-0df8-489a-a208-ab3ab091316d)
*The JUCE app displaying 6 lines of RMS/time (low/mid/high freq bands for 2 channels of audio input)*

Audiosapper pipes its input (what a user application would see as a speaker) to its output (what a user application would see as a microphone). So, if audiosapper is set as the system audio output device, any application can listen to the system audio as though it were the input coming from a microphone.

## Installation

Open and build NullAudio.xcodeproj in audiosapper_plugin. Take the plugin folder (by default generated in ~/Library/Developer/XCode/DerivedData/NullAudio*/Build/Products/NullAudio.driver) and place it into /Library/Audio/Plug-Ins/HAL. Either restart CoreAudio, or reboot your computer. Upon reboot, you will have an additional output device named "Null Audio". Open Audio MIDI Setup and create a Multi-Output Device including Null Audio and whatever other desired output device you want.

Open wavsurfer_app/wavsurfer.jucer with Projucer, open it in XCode, and run and build the app. Grant microphone access when requested, and enjoy the simple visualizer. 

## Future work
I need to add smoothing to the wavsurfer_app display as it is currently pretty jarring.

The audiosapper plugin has some distortion to input sound, especially apparent if you try to writemix audiosapper's output to multiple listeners (say JUCE + QuickTime player). The issue becomes drastically worse with multiple readers, and I am trying to figure out what is causing this. Maybe some real time constraints. 

## Credit
The audiosapper is largely based off of [Apples' Null Audio example](https://developer.apple.com/documentation/coreaudio/creating_an_audio_server_driver_plug-in/), with modifications to route output channels back to input channels: 

## License

Keep in mind wavsurfer_app uses JUCE, so their Licenses restrictions apply in that context

As for the source code of wavsurfer_app and audiosapper, MIT license on those.
[MIT](https://choosealicense.com/licenses/mit/). Make something cool
