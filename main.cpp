#include <iostream>
#include <opencv2/opencv.hpp>

#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "edge-impulse-sdk/dsp/image/image.hpp"

using namespace std;

#define IMAGE_PATH "/home/otavio/enoe_labels-cpp-mcu-v6_gabriel/dataset_reduzido/flood/20181110_163153-SHOP.jpg"

// // Buffer da imagem no formato RGB888 (uint8_t)
uint8_t image_buffer[EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT * 3];

// 🧠 Callback para preencher um pedaço do sinal com valores da imagem
static int get_signal_data(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        out_ptr[i] = static_cast<float>(image_buffer[offset + i]);
    }
    return 0; // EIDSP_OK
}

// Carrega e converte imagem em RGB888 plano
bool load_and_preprocess_image(const char *image_path) {
    cv::Mat image = cv::imread(image_path, cv::IMREAD_COLOR);
    if (image.empty()) {
        cerr << "Erro ao carregar imagem: " << image_path << endl;
        return false;
    }

    cout << "Tamanho original: " << image.cols << "x" << image.rows << endl;

    cv::Mat resized;
    cv::resize(image, resized, cv::Size(EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT));
    cout << "Redimensionado para: " << resized.cols << "x" << resized.rows << endl;

    int pixel_ix = 0;
    for (int y = 0; y < resized.rows; y++) {
        for (int x = 0; x < resized.cols; x++) {
            cv::Vec3b pixel = resized.at<cv::Vec3b>(y, x);
            image_buffer[pixel_ix++] = pixel[2]; // R
            image_buffer[pixel_ix++] = pixel[1]; // G
            image_buffer[pixel_ix++] = pixel[0]; // B
        }
    }

    return true;
}

int main() {
    printf("EI CLASSIFIER_INPUT_WIDTH: %d\n", EI_CLASSIFIER_INPUT_WIDTH);
    printf("EI CLASSIFIER_INPUT_HEIGHT: %d\n", EI_CLASSIFIER_INPUT_HEIGHT);
    ei_impulse_result_t result;

    if (!load_and_preprocess_image(IMAGE_PATH)) {
        return 1;
    }

    // Cria o sinal manualmente com callback
    signal_t signal;
    signal.total_length = sizeof(image_buffer);  // 96 x 96 x 3 = 27648
    signal.get_data = &get_signal_data;

    // Rodar inferência
    EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);

    if (res != EI_IMPULSE_OK) {
        cerr << "Erro na inferência: " << res << endl;
        return 1;
    }

    cout << "Inferência concluída com sucesso:\n";
    cout << "Tempo: DSP " << result.timing.dsp << " ms, Inference " << result.timing.classification << " ms\n";

    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        cout << "  " << ei_classifier_inferencing_categories[ix] << ": "
             << result.classification[ix].value << endl;
    }

#if EI_CLASSIFIER_HAS_ANOMALY == 1
    cout << "Anomaly score: " << result.anomaly << endl;
#endif

    return 0;
}
