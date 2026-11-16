#include <opencv2/opencv.hpp>

#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

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

    // Construct these once, outside the frame loop.
    auto orb = cv::ORB::create(1000);
    cv::BFMatcher matcher(cv::NORM_HAMMING, false);

    cv::Mat previousGray;
    cv::Mat previousDescriptors;
    std::vector<cv::KeyPoint> previousKeypoints;

    cv::Mat frame;

    while (cap.read(frame)) {
        const int64 start = cv::getTickCount();

        // Limit processing resolution while preserving aspect ratio.
        cv::Mat resized;
        const double scale = std::min(1.0, 960.0 / frame.cols);
        cv::resize(frame, resized, cv::Size(), scale, scale);

        cv::Mat gray;
        cv::cvtColor(resized, gray, cv::COLOR_BGR2GRAY);

        std::vector<cv::KeyPoint> keypoints;
        cv::Mat descriptors;
        orb->detectAndCompute(
            gray, cv::noArray(), keypoints, descriptors
        );

        std::vector<cv::DMatch> goodMatches;

        if (!previousDescriptors.empty() && descriptors.rows >= 2) {
            std::vector<std::vector<cv::DMatch>> neighbors;

            // For each previous descriptor, find its two nearest
            // descriptors in the current frame.
            matcher.knnMatch(
                previousDescriptors, descriptors, neighbors, 2
            );

            for (const auto& candidates : neighbors) {
                if (candidates.size() < 2)
                    continue;

                // Reject matches whose best candidate is too similar
                // in distance to the second-best candidate.
                if (candidates[0].distance <
                    0.75f * candidates[1].distance) {
                    goodMatches.push_back(candidates[0]);
                }
            }
        }

        const double processingMs =
            1000.0 * (cv::getTickCount() - start) /
            cv::getTickFrequency();

        cv::Mat visualization;

        if (!previousGray.empty()) {
            cv::drawMatches(
                previousGray, previousKeypoints,
                gray, keypoints,
                goodMatches, visualization,
                cv::Scalar::all(-1),
                cv::Scalar::all(-1),
                std::vector<char>(),
                cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS
            );
        } else {
            cv::drawKeypoints(gray, keypoints, visualization);
        }

        cv::putText(
            visualization,
            "Matches: " + std::to_string(goodMatches.size()) +
                " | Processing: " +
                std::to_string(static_cast<int>(processingMs)) + " ms",
            cv::Point(15, 25),
            cv::FONT_HERSHEY_SIMPLEX,
            0.6, cv::Scalar(0, 255, 0), 2
        );

        cv::imshow("ORB: previous frame -> current frame", visualization);

        // Reuse current results on the next iteration.
        previousGray = gray;
        previousDescriptors = descriptors;
        previousKeypoints = std::move(keypoints);

        if ((cv::waitKey(1) & 0xff) == 27)
            break;
    }

    return 0;
}