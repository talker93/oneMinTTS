//
//  Created by 江山 on 4/15/24.
//

#include <onnxruntime_cxx_api.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <fstream>
#include <iostream>
#include <chrono>
#include <filesystem>
#include <array>
#include <string>
#include <memory>
#include <vector>
#include <cctype>
#include <cstdio>
#include <stdexcept>
#include <cstdint>
#include <cctype> // For std::isspace

namespace fs = std::filesystem;

// Execute the shell command and return its output
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    #ifdef WINDOWS
        std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
    #else
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    #endif
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

// Function to write a WAV header to a file
void write_wav_header(std::ofstream& file, int sampleRate, int bitsPerSample, int numChannels, int numSamples) {
    file.write("RIFF", 4); // RIFF Header
    // Overall size of file in bytes (file size - 8 bytes):
    // 4 bytes for "WAVE", 24 for standard fmt chunk, 8 for data chunk header, numSamples * numChannels * bitsPerSample/8 for data
    int chunkSize = 36 + numSamples * numChannels * bitsPerSample / 8;
    file.write(reinterpret_cast<const char*>(&chunkSize), 4);
    file.write("WAVE", 4); // WAVE Header
    file.write("fmt ", 4); // fmt subchunk
    int subchunk1Size = 16; // Size of fmt chunk
    file.write(reinterpret_cast<const char*>(&subchunk1Size), 4);
    short audioFormat = 1; // Audio format 1=PCM
    file.write(reinterpret_cast<const char*>(&audioFormat), 2);
    file.write(reinterpret_cast<const char*>(&numChannels), 2);
    file.write(reinterpret_cast<const char*>(&sampleRate), 4);
    int byteRate = sampleRate * numChannels * bitsPerSample / 8;
    file.write(reinterpret_cast<const char*>(&byteRate), 4);
    short blockAlign = numChannels * bitsPerSample / 8;
    file.write(reinterpret_cast<const char*>(&blockAlign), 2);
    file.write(reinterpret_cast<const char*>(&bitsPerSample), 2);
    file.write("data", 4);
    int subchunk2Size = numSamples * numChannels * bitsPerSample / 8;
    file.write(reinterpret_cast<const char*>(&subchunk2Size), 4);
}

std::string cleanText(std::string &rawText) {
    std::string cleanText;
    for(char c : rawText) {
        if (!ispunct(static_cast<unsigned char>(c))) {
            cleanText += std::tolower(static_cast<unsigned char>(c));
        }
    }
    return cleanText;
}

int UTF8CharLength(char leadByte) {
    if ((leadByte & 0x80) == 0x00) {         // 0xxx xxxx
        return 1;
    } else if ((leadByte & 0xE0) == 0xC0) {  // 110x xxxx
        return 2;
    } else if ((leadByte & 0xF0) == 0xE0) {  // 1110 xxxx
        return 3;
    } else if ((leadByte & 0xF8) == 0xF0) {  // 1111 0xxx
        return 4;
    }
    return 0;  // Invalid UTF-8 byte
}

int main() {
    int token_max_length = 2000;
    fs::path exec_path = fs::current_path();
    
    // Step 0. Text Input & Cleaning
    // Specify the model
    std::string model_name;
    while (model_name.empty()) {
        std::cout << "Enter your model name: e.g. facebook/mms-tts-kor" << std::endl;
        std::getline(std::cin, model_name); // Reads a line of text from standard input
        
        // Now text_entry contains the text entered by the user
        std::cout << std::endl;
        std::cout << "You entered: " << model_name << std::endl;
    }
    
    // Load the vocabulary
    std::ifstream file_mms_vocab(exec_path / "assets/mms_vocab.json");
    nlohmann::json mms_vocab = nlohmann::json::parse(file_mms_vocab);
    nlohmann::json mms_vocab_cur = mms_vocab[model_name];
    // load the roman flag, some language need it some not
    bool roman_flag = mms_vocab_cur["is_uroman"];
    
    // The sentence to convert
    std::string text_entry;
    while (text_entry.empty()) {
        std::cout << "Enter your text: ";
        std::getline(std::cin, text_entry); // Reads a line of text from standard input
        
        // Now text_entry contains the text entered by the user
        std::cout << std::endl;
        std::cout << "You entered: " << text_entry << std::endl;
    }
    
    std::string clean_text = cleanText(text_entry);
    std::string roman_text;
    std::string cmd_roman;
    
    // Step 1. Romanizing
    if(roman_flag) {
        // Get the root folder
        #ifdef APPLE  // macOS
            cmd_roman = "perl ./assets/uroman/bin/uroman.pl" + std::string(" <<< ") + "'" + clean_text + "'";
        #elif defined(LINUX)  // Linux
            cmd_roman = "bash -c 'perl ./assets/uroman/bin/uroman.pl <<< \"" + clean_text + "\"'";
        #elif defined(WINDOWS)
            cmd_roman = "perl ./assets/uroman/bin/uroman.pl \"" + clean_text + "\"";
        #endif
        // Query the uroman library
        try {
            roman_text = exec(cmd_roman.c_str());
            roman_text.erase((std::remove(roman_text.begin(), roman_text.end(), '\n')),
                             roman_text.end()); // remove '\n'
            std::cout << "The script returned: " << roman_text << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Exception occurred: " << e.what() << std::endl;
        }
    } else {
        roman_text = clean_text;
    }
    
    // Step 2. Tokenization
    std::vector<int> vecTokens;
    vecTokens.push_back(0);
    for(size_t i = 0; i < roman_text.length(); ) {
        int len = UTF8CharLength(roman_text[i]);
        std::string cur_char = roman_text.substr(i, len);
        if(mms_vocab_cur.contains(cur_char)) {
            vecTokens.push_back(mms_vocab_cur[cur_char]);
            vecTokens.push_back(0);
        } else {
            std::cerr << cur_char << " is not found in the vocabulary." << std::endl;
        }
        i += len;
    }
    std::vector<int> vecAttentionMask(vecTokens.size(), 1); // init mask with ones
    // padding the inputs for tensor creation
    vecTokens.insert(vecTokens.end(),
                     token_max_length - vecTokens.size(), 0);
    vecAttentionMask.insert(vecAttentionMask.end(),
                            token_max_length-vecAttentionMask.size(), 0);
    int64_t* p_token_tensor = new int64_t[vecTokens.size()];
    int64_t* p_attention_mask = new int64_t[vecAttentionMask.size()];
    std::copy(vecTokens.begin(), vecTokens.end(), p_token_tensor);
    std::copy(vecAttentionMask.begin(), vecAttentionMask.end(), p_attention_mask);
    
    int64_t token_shape[] = {1, static_cast<int64_t>(vecTokens.size())};
    int64_t atten_shape[] = {1, static_cast<int64_t>(vecAttentionMask.size())};
    
    // Step 3. Inference
    // Init ONNX Runtime
    Ort::Env env_mms(ORT_LOGGING_LEVEL_WARNING, "test");
    Ort::SessionOptions session_options_mms;
    // session_options.SetIntraOpNumThreads(1); // set threads number as your wish
    
    // Load the ONNX model
    std::string local_model_name = model_name;
    std::replace(local_model_name.begin(), local_model_name.end(), '/', '_');
    fs::path model_path_mms = exec_path / ("assets/models/" + local_model_name + ".onnx");
    std::string model_path_mms_str = model_path_mms.string();
    std::cout << "Using model at: " << model_path_mms_str << std::endl;
    // session model path constructor
    #ifdef WINDOWS
        std::wstring wide_model_path_mms_str = std::wstring(model_path_mms_str.begin(), model_path_mms_str.end());
        const ORTCHAR_T* onnx_model_path = wide_model_path_mms_str.c_str();
    #else
        const ORTCHAR_T* onnx_model_path = model_path_mms_str.c_str();
    #endif
    
    Ort::Session session_mms(env_mms, onnx_model_path, session_options_mms);
    Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);
    
    // Prepare input and output names
    std::vector<const char*> input_names = {"input_ids", "attention_mask"};
    std::vector<const char*> output_names = {"waveform"};
    
    // Run the model
    std::vector<Ort::Value> input_tensors;
    input_tensors.push_back(Ort::Value::CreateTensor<int64_t>(memory_info, p_token_tensor, vecTokens.size(), token_shape, (int)sizeof(token_shape)/sizeof(token_shape[0])));
    input_tensors.push_back(Ort::Value::CreateTensor<int64_t>(memory_info, p_attention_mask, vecAttentionMask.size(), atten_shape, (int)sizeof(atten_shape)/sizeof(atten_shape[0])));
    
    auto start = std::chrono::high_resolution_clock::now();
    
    auto output_tensors = session_mms.Run(Ort::RunOptions{nullptr}, input_names.data(), input_tensors.data(), input_tensors.size(), output_names.data(), output_names.size());
    
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop-start);
    std::cout << "time spent: " << duration.count() << " microseconds" << std::endl;
    std::cout << std::endl;

    // Get output tensor
    auto& output_tensor = output_tensors.front();
    float* output_array = output_tensor.GetTensorMutableData<float>();
    size_t output_size = output_tensor.GetTensorTypeAndShapeInfo().GetElementCount();
    
    // Constants for WAV format - adjust these according to your audio data
    int sampleRate = 16000; // Sample rate
    int bitsPerSample = 16; // Bits per sample
    int numChannels = 1; // Number of channels
    int numSamples = (int)output_size; // Number of samples

    // Open file stream for output WAV file
    fs::path out_path = exec_path / (local_model_name+".wav");
    std::ofstream out_file(out_path, std::ios::binary);

    // Write WAV header
    write_wav_header(out_file, sampleRate, bitsPerSample, numChannels, numSamples);

    // Convert float samples to 16-bit integers (assuming PCM data is in -1.0 to 1.0 range)
    for (size_t i = 0; i < output_size; ++i) {
        // Clamp values to the range [-1.0, 1.0] and scale to 16-bit integer range
        float sample = std::max(-1.0f, std::min(1.0f, output_array[i]));
        short intSample = static_cast<short>(sample * 32767);
        out_file.write(reinterpret_cast<const char*>(&intSample), sizeof(short));
    }

    // Close file
    out_file.close();
    
    delete[] p_token_tensor;
    delete[] p_attention_mask;

    std::cout << "Jobs done." << std::endl;
    std::cout << "Output written to " << local_model_name << ".wav" << std::endl;
    
    return 0;
}
