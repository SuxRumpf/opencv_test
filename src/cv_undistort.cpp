#include "../include/helper_functions.h"
cv::Mat cameraMatrix(3, 3, CV_64F);
cv::Mat distCoeffs(1, 5, CV_64F);

void run_undistort_test_without_T_API() {
    auto start = std::chrono::high_resolution_clock::now();

    float intrinisic_tmp[9];
    float distortion_tmp[5];

    load_intrinsic_params(intrinisic_tmp, distortion_tmp);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            cameraMatrix.at<double>(i, j) = intrinisic_tmp[i * 3 + j];
        }
    }

    for (int i = 0; i < 5; i++) {
        distCoeffs.at<double>(0, i) = distortion_tmp[i];
    }


    //cv::VideoCapture cap = cv::VideoCapture(0, cv::CAP_GSTREAMER);
    cv::Mat img;
    cv::Mat dst0;
    std::cout << "INIT TIME: " << (std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - start).count()) << std::endl;

    //cv::imshow("sdsd image", img);
    //cv::waitKey(0);

    /*
     * Da undistort eine Verbindung aus initUndistortRectifyMap und remap ist, muss jedes mal initUndistortRectifyMap neu
     * ausgeführt werden weshalb hier große Performanceprobleme entstehen können
     */
    std::cout << "Normale Undistortion: " << std::endl;

    auto start_ges_loop_0 = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < 10; x++) {
        img = cv::imread("../test.png");

        auto start_undistort = std::chrono::high_resolution_clock::now();
        cv::undistort(img, dst0, cameraMatrix, distCoeffs);

        std::cout << "undistort: " << (std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now() - start_undistort).count()) << std::endl;
        std::cout << "Durchschnittliche Laufzeit der normalen Undistortion(" << x << ". iteration): " <<
                  (double) (std::chrono::duration_cast<std::chrono::microseconds>(
                          std::chrono::high_resolution_clock::now() - start_ges_loop_0).count()) / ((double) x + 1.0)
                  << std::endl;
    }


    cv::Mat dst1;
    /*
     * Hier wird nun initUndistortRectifyMap getrennt von remap und nur einmal vor dem Start aufgerufen
     */
    std::cout << "\n\nGesplittete Undistortion: " << std::endl;
    cv::Mat map1, map2;
    auto start_ges_loop_1 = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < 25; x++) {
        img = cv::imread("../test.png");

        if (x == 0) {
            auto start_initUndistortRectifyMap = std::chrono::high_resolution_clock::now();
            auto size_of_img = cv::Size(img.cols, img.rows);
            std::cout << "IMG Size: " << size_of_img << std::endl;
            cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(), cameraMatrix, size_of_img, CV_32FC1, map1,
                                        map2);
            std::cout << "initUndistortRectifyMap: " <<
                      (std::chrono::duration_cast<std::chrono::microseconds>(
                              std::chrono::high_resolution_clock::now() - start_initUndistortRectifyMap).count())
                      << std::endl;
        }

        auto start_undistort = std::chrono::high_resolution_clock::now();
        cv::remap(img, dst1, map1, map2, cv::INTER_CUBIC);
        std::cout << "undistort: " << (std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now() - start_undistort).count()) << std::endl;
        std::cout << "Durchschnittliche Laufzeit der getrennten Undistortion(" << x << ". iteration): " <<
                  (double) (std::chrono::duration_cast<std::chrono::microseconds>(
                          std::chrono::high_resolution_clock::now() - start_ges_loop_1).count()) / ((double) x + 1.0)
                  << std::endl;
    }

    /*
    * Hier wird wieder initUndistortRectifyMap getrennt von remap allerdings diesmal mit fixed point maps (CV_16SC2) statt CV_32FC1
    */
    std::cout << "\n\nGesplittete Undistortion: " << std::endl;
    auto start_ges_loop_2 = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < 25; x++) {
        img = cv::imread("../test.png");

        if (x == 0) {
            auto start_initUndistortRectifyMap = std::chrono::high_resolution_clock::now();
            auto size_of_img = cv::Size(img.cols, img.rows);
            std::cout << "IMG Size: " << size_of_img << std::endl;
            cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(), cameraMatrix, size_of_img, CV_16SC2, map1,
                                        map2);
            std::cout << "initUndistortRectifyMap: " <<
                      (std::chrono::duration_cast<std::chrono::microseconds>(
                              std::chrono::high_resolution_clock::now() - start_initUndistortRectifyMap).count())
                      << std::endl;
        }

        auto start_undistort = std::chrono::high_resolution_clock::now();
        cv::remap(img, dst1, map1, map2, cv::INTER_CUBIC);
        std::cout << "undistort: " << (std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now() - start_undistort).count()) << std::endl;
        std::cout << "Durchschnittliche Laufzeit der getrennten Undistortion(" << x << ". iteration): " <<
                  (double) (std::chrono::duration_cast<std::chrono::microseconds>(
                          std::chrono::high_resolution_clock::now() - start_ges_loop_2).count()) / ((double) x + 1.0)
                  << std::endl;
    }
}

void run_undistort_test_with_T_API(){
    /*
     * Normale undistort function aber mit T-API
     */
    std::cout << "Normale Undistortion mit T-API: " <<std::endl;
    cv::Mat img, map1, map2;

    cv::UMat uimg, udst0;
    auto start_ges_loop_3 = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < 25; x++) {
        cv::imread("../test.png").copyTo(uimg);

        auto start_undistort = std::chrono::high_resolution_clock::now();
        cv::undistort(uimg, udst0, cameraMatrix, distCoeffs);

        std::cout << "undistort mit T-API: " << (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_undistort).count())  << std::endl;
        std::cout << "Durchschnittliche Laufzeit der normalen Undistortion mit T-API(" << x <<". iteration): " <<
                  (double)(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_ges_loop_3).count()) / ((double)x+1.0) << std::endl;
    }

/*
 * initUndistortRectifyMap getrennt von remap und nur einmal aber mit T-API ohne umap1 umap2
 */
    std::cout << "\n\nGesplittete Undistortion: " <<std::endl;
    cv::UMat uimg1, udst1;
    auto start_ges_loop_4 = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < 25; x++) {
        cv::imread("../test.png").copyTo(uimg1);

        if (x == 0){
            auto start_initUndistortRectifyMap = std::chrono::high_resolution_clock::now();
            auto size_of_img = cv::Size(uimg1.cols, uimg1.rows);
            std::cout << "IMG Size: " << size_of_img << std::endl;
            cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(), cameraMatrix, size_of_img, CV_32FC1, map1, map2);
            std::cout << "initUndistortRectifyMap: " <<
                      (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_initUndistortRectifyMap).count())  << std::endl;
        }

        auto start_undistort = std::chrono::high_resolution_clock::now();
        cv::remap(uimg1, udst1, map1, map2, cv::INTER_CUBIC);
        std::cout << "undistort: " << (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_undistort).count())  << std::endl;
        std::cout << "Durchschnittliche Laufzeit der getrennten Undistortion mit T-API 1x(" <<x <<". iteration): " <<
                  (double)(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_ges_loop_4).count()) / ((double)x+1.0)  << std::endl;
    }

    /*
 * initUndistortRectifyMap getrennt von remap und nur einmal aber mit T-API mit umap1 umap2
 */
    std::cout << "\n\nGesplittete Undistortion: " <<std::endl;
    cv::UMat umap1, umap2;
    cv::UMat uimg2, udst2;
    auto start_ges_loop_5 = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < 50; x++) {
        cv::imread("../test.png").copyTo(uimg1);

        if (x == 0){
            auto start_initUndistortRectifyMap = std::chrono::high_resolution_clock::now();
            auto size_of_img = cv::Size(uimg1.cols, uimg1.rows);
            std::cout << "IMG Size: " << size_of_img << std::endl;
            cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(), cameraMatrix, size_of_img, CV_32FC1, umap1, umap2);
            std::cout << "initUndistortRectifyMap: " <<
                      (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_initUndistortRectifyMap).count())  << std::endl;
        }

        auto start_undistort = std::chrono::high_resolution_clock::now();
        cv::remap(uimg1, udst1, umap1, umap2, cv::INTER_CUBIC);
        std::cout << "undistort: " << (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_undistort).count())  << std::endl;
        std::cout << "Durchschnittliche Laufzeit der getrennten Undistortion mit T-API 2x(" <<x <<". iteration): " <<
                  (double)(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_ges_loop_5).count()) / ((double)x+1.0)  << std::endl;
    }

    cv::imshow("grayscale image", img);
    cv::waitKey(0);
}


int main(int argc, char *argv[]){
    run_undistort_test_without_T_API();
    return 0;
}