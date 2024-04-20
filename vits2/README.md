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
2. Open folder `build/<YOUR PC ARCH>` in terminal/cmd
```
cd oneMinTTS/build/mac
```
3. Start Synthesis
```
❯ ./oneMinTTS
Enter your text: hello world

......

Inferencing...
Using model at: /Users/shanjiang/Desktop/oneMinTTS/build/mac_arm64/vits.onnx
time spent: 160658 microseconds

Jobs done.
Output written to output.wav
```

## Documentation
The supported model is currently only VITS.
Check the training process from this repo: `https://github.com/p0p4k/vits2_pytorch`
More model's support is coming soon!

<!-- ## Contributing
We welcome contributions and suggestions! Please refer to the Contributing Guidelines for more information. -->

## License
This project is licensed under the MIT License - see the LICENSE file for details.