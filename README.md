# Voice Assistant App for Desktop (Qt5 C++)

### Requirements

* Operation system: **Windows** (support for other systems in future)
* Browser for open internet-resources: **Google Chrome**

### Assistant Skills 
*   **Build a routes on map taking into account traffic jams** (Used web version of Yandex Maps)
*   **Show the weather** for a place (e.g the weather in Moscow')
*   **Search for a file on the computer desktop by name**
*   **Opening the found file in the default application**
*   **Close the active window or file with a voice command**
*   **Minimize the active window or file with a voice command**
*   **Search for files of a certain type on the computer desktop** (For example, all jpg or png, all pdf or word, all mov or mp4 files.
*   **Open Internet resources** (such as google.com, yandex.ru, youtube.com, drive.google.com, translate.google.ru)
*   **Make a search query in the Google search engine**
*   **Make a search query in the Yandex search engine**
*   **Make a search query in the Youtube search engine**
*   **Close active browser tab on command**
*   **Writes the history of voice commands in the current time**  

In the future, the processing of voice commands will be expanded.

## Demo
![Demo gif](https://github.com/ilya-filatov-94/Voice-assistent/blob/main/presentation_app/gif_demonstration.gif)  

## Getting Started
### Create KEYs for third party APIs
Voice assistant uses third party APIs for speech recognition, building routes and weather forecasting.
All the following APIs have free no-commercial API calls. Subscribe to the following APIs in order to take FREE access KEYs.
*   [VK ASR API](https://vk.com/voice-tech): free API for speech recognition.
*   [Yandex SpeechKit](https://cloud.yandex.ru/docs/speechkit/): paid API with a free period of 2 months for speech recognition.
*   [Yandex GeoCoder](https://yandex.ru/dev/maps/geocoder/?from=mapsapi): API for current location and buildin routes.

Since the speech recognition APIs accept the ogg audio format, you will need to install the library to convert the audio.  
*   [FFmpeg](http://www.ffmpeg.org/download.html).  
*   This application was compiled using the MinGW 64bit compiler and tested on Windows 10.   
*   Using building system tool - qmake. 
You can watch the application in action on the video at the [link](https://github.com/ilya-filatov-94/Voice-assistent/blob/main/presentation_app/video_description.mp4)

