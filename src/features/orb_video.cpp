#include "feature_matching_and_detection.h"
int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: orb_video <video_path>\n";
        return 1;
    }

    cv::VideoCapture cap(argv[1]);

    if (!cap.isOpened()) {
        std::cerr << "Could not open video: " << argv[1] << '\n';
        return 1;
    }

    feature_detection_and_matching_nodes features_node;
    cv::Mat frame;

    while (cap.read(frame)) {
        const int64 start = cv::getTickCount();

        // Feature detection and matching.
        MatchingResult result =
            features_node.feature_detection_matching(frame);

        const double processingMs =
            1000.0 * (cv::getTickCount() - start) /
            cv::getTickFrequency();

        // ---------------- Visualization ----------------

        cv::Mat visualization;

        if (!result.previousGray.empty()) {
            cv::drawMatches(
                result.previousGray,
                result.previousKeypoints,
                result.gray,
                result.keypoints,
                result.goodMatches,
                visualization,
                cv::Scalar::all(-1),
                cv::Scalar::all(-1),
                std::vector<char>(),
                cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS
            );
        } else {
            cv::drawKeypoints(
                result.gray,
                result.keypoints,
                visualization
            );
        }

        cv::putText(
            visualization,
            "Matches: " + std::to_string(result.goodMatches.size()) +
                " | Processing: " +
                std::to_string(static_cast<int>(processingMs)) + " ms",
            cv::Point(15, 25),
            cv::FONT_HERSHEY_SIMPLEX,
            0.6,
            cv::Scalar(0, 255, 0),
            2
        );

        cv::imshow(
            "ORB: previous frame -> current frame",
            visualization
        );

        if ((cv::waitKey(1) & 0xff) == 27)
            break;
    }

    return 0;
}