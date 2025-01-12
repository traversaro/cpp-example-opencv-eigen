#include <iostream>

#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <rerun.hpp>

#include "batch_adapters.hpp"

std::vector<Eigen::Vector3f> generate_random_points_vector(int num_points) {
    std::vector<Eigen::Vector3f> points(num_points);
    for (auto& point : points) {
        point.setRandom();
    }
    return points;
}

int main() {
    const auto rec = rerun::RecordingStream("rerun_example_cpp");
    rec.spawn().exit_on_failure();

    rec.log_timeless("world", rerun::ViewCoordinates::RIGHT_HAND_Z_UP); // Set an up-axis

    const int num_points = 1000;

    // Points represented by std::vector<Eigen::Vector3f>
    const auto points3d_vector = generate_random_points_vector(1000);
    rec.log("world/points_from_vector", rerun::Points3D(points3d_vector));

    // Points represented by Eigen::Mat3Xf (3xN matrix)
    const Eigen::Matrix3Xf points3d_matrix = Eigen::Matrix3Xf::Random(3, num_points);
    rec.log("world/points_from_matrix", rerun::Points3D(points3d_matrix));

    // Posed pinhole camera:
    rec.log(
        "world/camera",
        rerun::Pinhole::from_focal_length_and_resolution({500.0, 500.0}, {640.0, 480.0})
    );

    const Eigen::Vector3f camera_position{0.0, -1.0, 0.0};
    Eigen::Matrix3f camera_orientation;
    // clang-format off
    camera_orientation <<
        +1.0, +0.0, +0.0,
        +0.0, +0.0, +1.0,
        +0.0, -1.0, +0.0;
    // clang-format on
    rec.log(
        "world/camera",
        rerun::Transform3D(
            rerun::datatypes::Vec3D(camera_position.data()),
            rerun::datatypes::Mat3x3(camera_orientation.data())
        )
    );

    // Read image
    const auto image_path = "rerun-logo.png";
    cv::Mat img = imread(image_path, cv::IMREAD_COLOR);
    if (img.empty()) {
        std::cout << "Could not read the image: " << image_path << std::endl;
        return 1;
    }

    // Log image to Rerun
    cv::cvtColor(img, img, cv::COLOR_BGR2RGB); // Rerun expects RGB format
    // NOTE currently we need to construct a vector to log an image, this will change in the future
    //  see https://github.com/rerun-io/rerun/issues/3794
    std::vector<uint8_t> img_vec(img.total() * img.channels());
    img_vec.assign(img.data, img.data + img.total() * img.channels());
    rec.log(
        "image",
        rerun::Image(
            {static_cast<size_t>(img.rows),
             static_cast<size_t>(img.cols),
             static_cast<size_t>(img.channels())},
            std::move(img_vec)
        )
    );

    return 0;
}
