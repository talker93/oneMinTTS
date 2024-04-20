from transformers import VitsModel, AutoTokenizer
import torch
import os

base_dir = os.path.dirname(os.path.realpath(__file__))

# Define the desired models list
model_names = ["facebook/mms-tts-eng", "facebook/mms-tts-kor", "facebook/mms-tts-ara", "facebook/mms-tts-deu", "facebook/mms-tts-hin", 
                "facebook/mms-tts-spa", "facebook/mms-tts-mal", "facebook/mms-tts-kan", "facebook/mms-tts-fra", "facebook/mms-tts-rus"]
# model_names = ["facebook/mms-tts-swe", "facebook/mms-tts-tur", "facebook/mms-tts-kir", "facebook/mms-tts-uzb-script_cyrillic", "facebook/mms-tts-pol",
#                "facebook/mms-tts-fas", "razhan/mms-tts-ckb", "facebook/mms-tts-eng-train", "facebook/mms-tts-amh",
#                "facebook/mms-tts-ell", "facebook/mms-tts-ory", "facebook/mms-tts-aka", "facebook/mms-tts-mlg", "facebook/mms-tts-ukr-finetune-v1-test",
#                "facebook/mms-tts-kmr-script_latin", "facebook/mms-tts-ukr", "facebook/mms-tts-ewe", "facebook/mms-tts-bam", "Matthijs/mms-tts-gbm",
#                "facebook/mms-tts-hun", "facebook/mms-tts-uig-script_arabic", "facebook/mms-tts-swh", "facebook/mms-tts-guj-train", "Matthijs/mms-tts-urd-script_devanagari",
#                "facebook/mms-tts-hak", "facebook/mms-tts-kaz", "facebook/mms-tts-tam-train", "facebook/mms-tts-nld", "Matthijs/mms-tts-bul",
#                "facebook/mms-tts-cpu"]
# model_names = ["facebook/mms-tts-fao", "facebook/mms-tts-bod", "ylacombe/mms-tts-spain-train", "facebook/mms-tts-hau", "Matthijs/mms-tts-deu",
#                "Matthijs/mms-tts-kor", "facebook/mms-tts-tgl", "facebook/mms-tts-shn", "vbrydik/mms-tts-ukr-train",
#                "facebook/mms-tts-kmr-script_arabic", "facebook/mms-tts-azb", "facebook/mms-tts-ceb", "Matthijs/mms-tts-mon",
#                "chuubjak/mms-tts-thai", "facebook/mms-tts-khm", "facebook/mms-tts-rif-script_arabic", "facebook/mms-tts-mya", "Matthijs/mms-tts-jam",
#                "facebook/mms-tts-kff-script_telugu", "facebook/mms-tts-sun", "facebook/mms-tts-sna", "facebook/mms-tts-ukr-finetune-v0", "Matthijs/mms-tts-itl",
#                "facebook/mms-tts-kik", "facebook/mms-tts-mos", "facebook/mms-tts-ace", "facebook/mms-tts-che",
#                "Matthijs/mms-tts-hyw", "Matthijs/mms-tts-kss", "Matthijs/mms-tts-tgk"]
print(len(model_names), " models are being download.")

if __name__ == "__main__":
    for model_name in model_names:
        try:
            max_token_length = 2000

            # Init a tokenizer for tensor creation
            tokenizer = AutoTokenizer.from_pretrained(model_name)
            inputs = tokenizer(text="", return_tensors="pt")

            input_ids = [0] * max_token_length
            attention_mask = [1] * max_token_length
            # dimension adaptation for onnx, 1-d -> 2-d
            input_ids = torch.tensor(input_ids).unsqueeze(0)
            attention_mask = torch.tensor(attention_mask).unsqueeze(0)

            inputs.data['input_ids'] = input_ids
            inputs.data['attention_mask'] = attention_mask

            # Load the model
            model = VitsModel.from_pretrained(model_name)

            # you can change the speech speed or noise level
            # model.speaking_rate = 1.0
            # model.noise_scale = 0.8

            with torch.no_grad():
                outputs = model(**inputs)

            # Define the directory to save the models        
            model_dir = os.path.join(base_dir, "models")
            os.makedirs(model_dir, exist_ok=True)
            onnx_file_path = os.path.join(model_dir, model_name.replace("/", "_")+".onnx")

            # Export the model to ONNX format
            input_names = ['input_ids', 'attention_mask']
            output_names = ['waveform']
            torch.onnx.export(model, tuple(inputs.values()), onnx_file_path, input_names=input_names, output_names=output_names)
            print("model saved: ", model_name)
            print("model index: ", model_names.index(model_name))
        except Exception as e:
            print("failed on exporting model: ", model_name)
            print("error reason: ", e)

