#include <opencv2/opencv.hpp>
#include <iostream>
int main() {
    cv::Mat img = cv::imread("/home/otavio/enoe_labels-cpp-mcu-v6_gabriel/dataset_reduzido/flood/20181110_163153-SHOP.jpg");
    if (img.empty()) {
        std::cout << "Falha ao carregar imagem\n";
        return 1;
    }
    std::cout << "Imagem carregada com sucesso: " << img.cols << "x" << img.rows << "\n";
    return 0;
}
