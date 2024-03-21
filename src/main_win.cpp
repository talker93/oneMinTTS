#include <vector>
#include <onnxruntime_cxx_api.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <filesystem>

#include <array>
#include <string>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <cctype>
#include <cstdio>
#include <stdexcept>
#include <cstdint>
#include <cctype> // For std::isspace


namespace fs = std::filesystem;

// execute command and pipe the prints back
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

std::string addSpaceBetweenWordsAndPunctuation(const std::string& input) {
    std::string result;
    for (size_t i = 0; i < input.length(); ++i) {
        char current = input[i];
        // Check if current character is punctuation and there is no space before it
        if (ispunct(current) && (i == 0 || !std::isspace(input[i - 1]))) {
            result += ' '; // Add a space before the punctuation
        }
        result += current;
        // Check if the current character is punctuation and it's not the last character,
        // and there is no space after it.
        if (ispunct(current) && (i + 1 < input.length()) && !std::isspace(input[i + 1])) {
            result += ' '; // Add a space after the punctuation
        }
    }
    return result;
}

// Function to load the ID dictionary from a text file
std::unordered_map<std::string, std::string> loadIdDictionary(const std::string& filename) {
    std::unordered_map<std::string, std::string> dictionary;
    std::ifstream file(filename);
    std::string line, word, id;

    while (getline(file, line)) {
        std::istringstream lineStream(line);
        if (getline(lineStream, word, ',') && getline(lineStream, id)) {
            dictionary[word] = id;
        }
    }

    return dictionary;
}

// Function to convert text into ID
std::vector<int64_t> textToId(const std::string& text, const std::unordered_map<std::string, std::string>& dictionary, int& wordCount) {

    std::string spacedString = addSpaceBetweenWordsAndPunctuation(text);
    std::istringstream textStream(spacedString);
    std::string word, ids;

    while (textStream >> word) {
        // Convert to lowercase
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        auto it = dictionary.find(word);
        if (it != dictionary.end()) {
            ids += it->second + " ";
        }
    }

    // Remove carriage, double quotes, brackets and returns from ids
    ids.erase(std::remove(ids.begin(), ids.end(), '\r'), ids.end());
    ids.erase(std::remove(ids.begin(), ids.end(), '"'), ids.end());
    ids.erase(std::remove(ids.begin(), ids.end(), '['), ids.end());
    for (char& ch : ids) {
        if (ch == ']') {
            ch = ',';
        }
    }

    // Remove extra spaces from ids
    std::string::iterator new_end2 = std::unique(ids.begin(), ids.end(), [](unsigned char a, unsigned char b) {
        return std::isspace(a) && std::isspace(b);
        });
    ids.erase(new_end2, ids.end());

    //    std::cout << "Id sequence: " << ids << std::endl;

    int commaCount = 0;
    for (char ch : ids) {
        if (ch == ',') {
            commaCount++;
        }
    }

    // Use stringstream to parse the numbers
    std::vector<int64_t> array;
    std::stringstream ss(ids);
    std::string token;
    while (std::getline(ss, token, ',') && wordCount < commaCount) {
        // Remove spaces
        token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
        // Convert string to int64_t and add to array
        array.push_back(std::stoll(token));
        wordCount++;
    }

    return array;
}


// Function to load the phoneme dictionary from a text file
std::unordered_map<std::string, std::string> loadPhonemeDictionary(const std::string& filename) {
    std::unordered_map<std::string, std::string> dictionary;
    std::ifstream file(filename);
    std::string line, word, phoneme;

    while (getline(file, line)) {
        std::istringstream lineStream(line);
        if (getline(lineStream, word, ',') && getline(lineStream, phoneme)) {
            dictionary[word] = phoneme;
        }
    }

    return dictionary;
}

// Function to convert text to phonemes using the dictionary
std::string textToPhonemes(const std::string& text, const std::unordered_map<std::string, std::string>& dictionary) {
    std::istringstream textStream(text);
    std::string word, phonemeText;
    while (textStream >> word) {
        // Remove punctuation from the end of words
        word.erase(std::remove_if(word.begin(), word.end(), ::ispunct), word.end());
        // Convert to lowercase
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        // query the dict to get phonemes
        auto it = dictionary.find(word);
        if (it != dictionary.end()) {
            phonemeText += it->second + " ";
        }
        else {
            phonemeText += word + " "; // Keep the word as is if not found
        }
    }
    return phonemeText;
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

int main() {
    // The sentence to convert
    std::string text_entry;
    while (text_entry.empty()) {
        std::cout << "Enter your text: ";
        std::getline(std::cin, text_entry); // Reads a line of text from standard input

        // Now text_entry contains the text entered by the user
        std::cout << std::endl;
        std::cout << "You entered: " << text_entry << std::endl;
    }

    // Load the phoneme dictionary
    fs::path exec_path = fs::current_path();
    std::cout << exec_path << std::endl;

    // Load the ID dictionary
    fs::path id_path = exec_path / "word_id_dict.csv";
    auto idDictionary = loadIdDictionary(id_path.string());

    std::cout << std::endl;
    std::cout << "Dictionary loaded at: " << id_path << std::endl;

    std::cout << std::endl;
    std::cout << "Phonemizing..." << std::endl;

    std::cout << std::endl;
    std::cout << "Tokenizing..." << std::endl;

    // Text to ids
    int elementCount = 0;
    std::vector<int64_t> iVecCache;
    iVecCache = textToId(text_entry, idDictionary, elementCount);

    //// Convert text to phonemes
    //std::string phonemes = textToPhonemes(text_entry, phonemeDictionary);

    //// Remove carriage returns from phonemes
    //phonemes.erase(std::remove(phonemes.begin(), phonemes.end(), '\r'), phonemes.end());

    //// Remove extra spaces from phonemes
    //std::string::iterator new_end = std::unique(phonemes.begin(), phonemes.end(), [](unsigned char a, unsigned char b) {
    //    return std::isspace(a) && std::isspace(b);
    //    });
    //phonemes.erase(new_end, phonemes.end());
    //// Trim trailing whitespace from 'phonemes' string
    //auto endpos = phonemes.find_last_not_of(" \t"); // Look for the last character that is not a space or tab
    //if (std::string::npos != endpos) {
    //    phonemes = phonemes.substr(0, endpos + 1); // Trim the string by cutting off everything after the last non-whitespace character
    //}
    //std::cout << "Phonemes: " << phonemes << std::endl;

    //std::cout << std::endl;
    //std::cout << "Tokenizing..." << std::endl;

    //// Convert phonemes into integers
    //fs::path script_path = exec_path / "sym2int.exe";
    //// std::string txt = "ɪn ɐ ɹˈɛlm wˌeə tˈaɪm wˈiːvz"; // for testing purpose
    //std::string command = script_path.string() + " \"" + phonemes + "\"";
    //std::string cmd_output;
    //try {
    //    cmd_output = exec(command.c_str());
    //    std::cout << "The script returned (token of characters): " << cmd_output << std::endl;
    //    std::cout << std::endl;
    //}
    //catch (const std::exception& e) {
    //    std::cerr << "Exception occurred: " << e.what() << std::endl;
    //}

    //int commaCount = 0;
    //for (char ch : cmd_output) {
    //    if (ch == ',') {
    //        commaCount++;
    //    }
    //}

    //std::vector<int64_t> array;
    //int elementCount = 0; // Keep track of the actual number of elements

    //// Remove the brackets at the beginning and the end if present
    //if (cmd_output.front() == '[') {
    //    cmd_output.erase(cmd_output.begin());
    //}
    //if (cmd_output.back() == ']') {
    //    cmd_output.pop_back();
    //}

    //// Use stringstream to parse the numbers
    //std::stringstream ss(cmd_output);
    //std::string token;
    //while (std::getline(ss, token, ',') && elementCount < commaCount + 1) {
    //    // Remove spaces
    //    token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
    //    // Convert string to int64_t and add to array
    //    array.push_back(std::stoll(token));
    //    elementCount++;
    //}

    // Dynamically allocate memory for the text array
    int64_t* text = new int64_t[elementCount];
    for (int i = 0; i < elementCount; i++) {
        text[i] = iVecCache[i];
    }
    std::cout << "ID sequence: ";
    for (const auto id : iVecCache) { std::cout << id << ", "; }
    std::cout << std::endl;
    std::cout << "Number of words: " << elementCount << std::endl;
    std::cout << std::endl;
    std::cout << "Inferencing..." << std::endl;

    // Initialize ONNX Runtime
    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");
    Ort::SessionOptions session_options;
    //session_options.SetIntraOpNumThreads(4);

    // Load the ONNX model
    fs::path fs_model_path = exec_path / "vits.onnx"; // Renamed variable to avoid conflict
    std::string model_path_str = fs_model_path.string();
    std::cout << "Using model at: " << model_path_str << std::endl;

    #ifdef _WIN32
        // Convert std::string (char) to std::wstring (wchar_t)
        std::wstring wide_model_path_str = std::wstring(model_path_str.begin(), model_path_str.end());
        const ORTCHAR_T* onnx_model_path = wide_model_path_str.c_str();
    #else
        const ORTCHAR_T* onnx_model_path = model_path_str.c_str();
    #endif
    // const ORTCHAR_T* onnx_model_path = L"C:\\Users\\jiang\\Downloads\\vits4local\\vits4local\\vits2.onnx"; // Example of a literal path
    Ort::Session session(env, onnx_model_path, session_options); // Use the new variable name here

    // Create mem info
    Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);

    // Set up input data for 'input'
    //int64_t text[] = { 102,56,16,70,16,123,156,86,54,55,16,65,157,86,123,16,62,156,43,102,55,16,65};
    int64_t text_len = elementCount;
    int64_t text_shape[] = { 1, text_len};  // Shape for 'input'

    // Set up input data for 'input_lengths'
    int64_t text_lengths[] = { text_len };
    int64_t text_lengths_shape[] = { 1 };  // Shape for 'input_lengths'

    // Set up input data for 'scales'
    float scales[] = { 0.667, 1.0, 0.8 };
    int64_t scales_shape[] = { 3 };  // Shape for 'scales'

    // Prepare input and output names
    std::vector<const char*> input_names = { "input", "input_lengths", "scales" }; // Match these with actual model input names
    std::vector<const char*> output_names = { "output" };  // Match these with actual model output names

    // Run the model
    std::vector<Ort::Value> input_tensors;
    input_tensors.push_back(Ort::Value::CreateTensor<int64_t>(memoryInfo, text, elementCount, text_shape, 2));
    input_tensors.push_back(Ort::Value::CreateTensor<int64_t>(memoryInfo, text_lengths, 1, text_lengths_shape, 1));
    input_tensors.push_back(Ort::Value::CreateTensor<float>(memoryInfo, scales, 3, scales_shape, 1));

    auto start = std::chrono::high_resolution_clock::now();

    auto output_tensors = session.Run(Ort::RunOptions{ nullptr }, input_names.data(), input_tensors.data(), input_tensors.size(), output_names.data(), output_names.size());

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "time spent: " << duration.count() << " microseconds" << std::endl;

    // Get output tensor
    auto& output_tensor = output_tensors.front();
    float* output_array = output_tensor.GetTensorMutableData<float>();
    size_t output_size = output_tensor.GetTensorTypeAndShapeInfo().GetElementCount();

    // Constants for WAV format - adjust these according to your audio data
    int sampleRate = 22050; // Sample rate
    int bitsPerSample = 16; // Bits per sample
    int numChannels = 1; // Number of channels
    int numSamples = (int)output_size; // Number of samples

    // Open file stream for output WAV file
    // This is the relative path corresponding to your current location
    fs::path out_path = exec_path / "output.wav";
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

    delete[] text;

    std::cout << "Output written to: " << out_path << std::endl;

    return 0;
}
