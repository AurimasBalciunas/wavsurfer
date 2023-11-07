# Wavsurfer

Wavsurfer is a combination of a macOS Core Audio server plug-in that pipes input back as its output (audiosapper_plugin), and a JUCE app that conducts a FFT on the input signal, divides it into low/med/high frequency bins and displays the sampled RMS of each bin over time for each audio channel, in real time.
![JUCE](https://github.com/AurimasBalciunas/wavsurfer/assets/56936689/47616800-3fc2-4885-b6e6-d49f4abee733)
*The JUCE app displaying 6 lines of RMS/time. One can see the low/mid/high freq bands for both channels of audio input. The left input bins are drawing and decaying from the left, and the right input channels are drawing and decaying from the right. Looks better live* 😄

Audiosapper pipes its input (what a user application would see as a speaker) to its output (what a user application would see as a microphone). So, if audiosapper is set as the system audio output device, any application can listen to the system audio as though it were the input coming from a microphone.

## Installation

Open and build NullAudio.xcodeproj in audiosapper_plugin. Take the plugin folder (by default generated in ~/Library/Developer/XCode/DerivedData/NullAudio*/Build/Products/NullAudio.driver) and place it into /Library/Audio/Plug-Ins/HAL. Either restart CoreAudio, or reboot your computer. Upon reboot, you will have an additional output device named "Null Audio". Open Audio MIDI Setup and create a Multi-Output Device including Null Audio and whatever other desired output device you want.

Open wavsurfer_app/wavsurfer.jucer with Projucer, open it in XCode, and run and build the app. Grant microphone access when requested, and enjoy the simple visualizer. 

## Future work

The audiosapper plugin has some distortion to input sound, especially apparent if you try to writemix audiosapper's output to multiple listeners (say JUCE + QuickTime player). The issue becomes drastically worse with multiple readers, and I am trying to figure out what is causing this. Maybe some real time constraints. 

## Credit
The audiosapper is largely based off of [Apples' Null Audio example](https://developer.apple.com/documentation/coreaudio/creating_an_audio_server_driver_plug-in/), with modifications to route output channels back to input channels: 

## License

Keep in mind wavsurfer_app uses JUCE, so their Licenses restrictions apply in that context

As for the source code of wavsurfer_app and audiosapper, MIT license on those.
[MIT](https://choosealicense.com/licenses/mit/). Make something cool
