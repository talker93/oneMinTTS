# oneMinTTS
Welcome to the GitHub repository for oneMinTTS! This project provides a simple solution for speech synthesis and model management, designed for high-performance and universal application. Below are some of the key features:

## Key Features

- **ONNX Model Support**: Seamlessly import ONNX models, including self-trained ones, enabling a wide range of applications and research in voice synthesis.

- **High-Performance C++**: Entirely written in C++, this project offers high performance and is crafted for efficiency and speed.

- **Self-Contained**: Comes with all necessary libraries included. There's no need for additional installations, making setup and operation as straightforward as possible.

- **Superior Quality**: Provides superior audio quality compared to Tacotron, enhancing user experience and output fidelity.

- **VITS Compatibility**: While based on various TTS Python projects, this implementation primarily serves the VITS model, ensuring compatibility and ease of use.

- **Cross-Platform Support**: Fully supports both macOS and Windows PC, ensuring broad accessibility and user flexibility.

## Getting Started
To begin using oneMinTTS, simply clone this repository to your local machine. Given the all-inclusive nature of the project, you can dive straight into importing your ONNX models or experimenting with pre-trained VITS models without the hassle of additional setups.

1. Clone repo
```
git clone https://github.com/talker93/oneMinTTS.git
```
2. Create the build folder
```
cd mms_tts && mkdir build && cd build
```
3. Compilation
```
cmake .. && make && cd fewByte
```
4. Start your synthesis
```
❯ ./fewByteTTS
Enter your model name: e.g. facebook/mms-tts-kor
facebook/mms-tts-ara

You entered: facebook/mms-tts-ara
Enter your text: في صمت الليل، أراقب النجوم، وأسمع همسات الريح تحكي عنك. قلبي ينبض بأسماء لا تُنسى، وذكرياتك تطفو كلمع البرق في سمائي. ظلك الممتد عبر الأزمان، يعانق روحي في لحظة سكون. أبحث عنك في كل زاوية، في كل شذا يعبق من الحدائق المعطرة. سألتقي بك عند منعطف القدر، حيث الأحلام تلتقي بالواقع. في انتظار الفجر الجديد، حيث يتجدد عهدنا مع كل شروق.

You entered: في صمت الليل، أراقب النجوم، وأسمع همسات الريح تحكي عنك. قلبي ينبض بأسماء لا تُنسى، وذكرياتك تطفو كلمع البرق في سمائي. ظلك الممتد عبر الأزمان، يعانق روحي في لحظة سكون. أبحث عنك في كل زاوية، في كل شذا يعبق من الحدائق المعطرة. سألتقي بك عند منعطف القدر، حيث الأحلام تلتقي بالواقع. في انتظار الفجر الجديد، حيث يتجدد عهدنا مع كل شروق.
Using model at: /Users/shanjiang/Desktop/oneMinTTS/mms_tts/build/fewByte/assets/models/facebook_mms-tts-ara.onnx
time spent: 6057903 microseconds

Jobs done.
Output written to facebook_mms-tts-ara.wav
```

## Huggingface Models Export
Due to the GitHub file size limitation, this project only contains 3 basic model for English, Korean and Arabic.
To download more models from huggingface, please do as below:
1. Install Huggingface Dependencies
```
pip install transformers==4.39.1
pip install torch==2.2.1
pip install onnx==1.14.1
```
2. Choose your perspective models
Modify the list `model_name` in `mms_tts/files/model_download.py`.
A comprehensive model list is available on `https://dl.fbaipublicfiles.com/mms/misc/language_coverage_mms.html`.
```
model_names = ["facebook/mms-tts-eng", "facebook/mms-tts-kor", "facebook/mms-tts-ara", "facebook/mms-tts-deu", "facebook/mms-tts-hin", 
                "facebook/mms-tts-spa", "facebook/mms-tts-mal", "facebook/mms-tts-kan", "facebook/mms-tts-fra", "facebook/mms-tts-rus"]
```
3. Export
Models are exported to `mms_tts/files/models`
```
python files/model_download.py
```

## VITS2
To utilize the vanila VITS2 model, please go to folder `vits2` for further instruction.

## Contributing
We welcome contributions and suggestions! 
Feel free to bring out any questions in Issues.
<!-- Please refer to the Contributing Guidelines for more information. -->

## License
This project is licensed under the MIT License - see the LICENSE file for details.
