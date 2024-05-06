<!-- # oneMinTTS -->
Welcome to the **oneMinTTS** GitHub repository! 
This project is engineered for high-performance speech synthesis and streamlined model management, now featuring support for HuggingFace 🤗 MMS-TTS models!

## 🔑 Key Features
- **小/Light-Weight**: A sleek executable under 200kB simplifies setup with all libraries included, perfect for easy deployment and rapid distribution of ONNX voice synthesis applications.

- **快/Agile-Dev**: Designed for agility with one C++ file for model inference and one Python file for model export, facilitating quick prototyping from text input to waveform output.

- **灵/Flex-Compatibility**: Fully compatible across Windows, macOS, and Linux, ideal for both desktop and server environments.

## 🚀 Getting Started
To begin using oneMinTTS, simply clone this repository to your local machine. Given the all-inclusive nature of the project, you can dive straight into importing your ONNX models or experimenting with pre-trained VITS models without the hassle of additional setups.

1. **Clone the Repository**
```bash
git clone https://github.com/talker93/oneMinTTS.git
```
2. **Prepare the Environment**
```bash
cd oneMinTTS/mms_tts && mkdir build && cd build
cmake .. && make
```
3. **Run the Synthesizer**
```bash
cd fewByte
./fewByteTTS
```

### 💡 Quick Example
```bash
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

## 🤗 Huggingface Models Export
This project includes three basic models due to GitHub's file size limits. For additional models:
1. Install Huggingface Dependencies
```
pip install transformers==4.39.1 torch==2.2.1 onnx==1.14.1
```
2. Select Models

Edit `model_name` in `mms_tts/files/model_download.py` with desired models from [this list](https://dl.fbaipublicfiles.com/mms/misc/language_coverage_mms.html).
```
model_names = ["facebook/mms-tts-eng", "facebook/mms-tts-kor", "facebook/mms-tts-ara", "facebook/mms-tts-deu", "facebook/mms-tts-hin", 
                "facebook/mms-tts-spa", "facebook/mms-tts-mal", "facebook/mms-tts-kan", "facebook/mms-tts-fra", "facebook/mms-tts-rus"]
```
3. Export Models
```
python files/model_download.py
```

## VITS2
For using the vanilla VITS2 model, refer to the vits2 folder instructions.

## 🤝 Contributing
Contributions are welcome! Feel free to post questions and suggestions in Issues.
<!-- Please refer to the Contributing Guidelines for more information. -->

## 📄 License
Licensed under the MIT License - see the LICENSE file for details.
