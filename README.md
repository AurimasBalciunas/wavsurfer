# Wavsurfer

Wavsurfer is a combination of an macOS audio server plug-in that pipes system audio back as a microphone input (audiosapper_plugin), and a simple JUCE app to show the potential of access to the system audio stream by calculating and displaying a smoothed RMS of the audio stream (wavsurfer_app).  

## Installation

Open and build NullAudio.xcodeproj in audiosapper_plugin. Take the plugin folder (by default generated in ~/Library/Developer/XCode/DerivedData/NullAudio*/Build/Products/NullAudio.driver) and place it into /Library/Audio/Plug-Ins/HAL. Either restart CoreAudio, or reboot your computer. Upon reboot, you will have an additional output device named "Null Audio". Open Audio MIDI Setup and create a Multi-Output Device including Null Audio and whatever other desired output device you want.

Open wavsurfer_app/wavsurfer.jucer with Projucer, open it in XCode, and run and build the app. Grant microphone access when requested, and enjoy your simple visualizer. 

## Usage

You can modify the smoothing factor in the range of (0, 1] in wavsurfer_app/MainComponent.cpp to tune the output wave as desired.  

## Future work
The audiosapper plugin has some distortion to input sound, especially apparent if you try to it through quicktime player. There are pops here and there in the output, which I *think* have to do with buffer underflow or other real-time constraints, but is something I am working to improve. 

Audiosapper is also currently a huge C file, I hope to convert the plugins to C++.

Finally, some physics calculations in the JUCE example to have the surfer catch some air could be a fun addition, but is less of a technical challenge than modifications to the CoreAudio plugin.

## Credit
The audiosapper is largely based off of [Apples' Null Audio example](https://developer.apple.com/documentation/coreaudio/creating_an_audio_server_driver_plug-in/), with modifications to route output channels back to input channels: 

## License

Keep in mind wavsurfer_app uses JUCE, so their Licenses restrictions apply in that context

As for the source code of wavsurfer_app and audiosapper, MIT license on those.
[MIT](https://choosealicense.com/licenses/mit/). Make something cool
