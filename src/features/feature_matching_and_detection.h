#pragma once

#include <opencv2/opencv.hpp>
#include <algorithm>
#include <iostream>
#include <vector>
#include <array>
#include <cstdint>

struct Point {
    float x;
    float y;
    float d;
    float z;
};

struct Feature {
    std::array<uint8_t, 32> descriptor;
    Point point;
    std::vector<int> matches;
};

// Everything needed to visualize the matches.
struct MatchingResult {
    cv::Mat gray;
    cv::Mat previousGray;

    std::vector<cv::KeyPoint> keypoints;
    std::vector<cv::KeyPoint> previousKeypoints;

    std::vector<cv::DMatch> goodMatches;
};

class feature_detection_and_matching_nodes {
public:
    std::vector<Feature> previous_features;

    cv::Ptr<cv::ORB> orb = cv::ORB::create(1000);
    cv::BFMatcher matcher{cv::NORM_HAMMING, false};

    cv::Mat previousGray;
    cv::Mat previousDescriptors;
    std::vector<cv::KeyPoint> previousKeypoints;

    MatchingResult feature_detection_matching(const cv::Mat& frame) {
        MatchingResult result;

        // Store the previous frame's information for visualization.
        result.previousGray = previousGray;
        result.previousKeypoints = previousKeypoints;

        // Resize while preserving aspect ratio.
        cv::Mat resized;
        const double scale = std::min(1.0, 960.0 / frame.cols);
        cv::resize(frame, resized, cv::Size(), scale, scale);

        cv::cvtColor(resized, result.gray, cv::COLOR_BGR2GRAY);

        cv::Mat descriptors;

        // Detect features and compute descriptors.
        orb->detectAndCompute(
            result.gray,
            cv::noArray(),
            result.keypoints,
            descriptors
        );

        // Match previous features to current features.
        if (!previousDescriptors.empty() && descriptors.rows >= 2) {
            std::vector<std::vector<cv::DMatch>> neighbors;

            matcher.knnMatch(
                previousDescriptors,
                descriptors,
                neighbors,
                2
            );

            // Lowe's ratio test.
            for (const auto& candidates : neighbors) {
                if (candidates.size() < 2)
                    continue;

                if (candidates[0].distance <
                    0.75f * candidates[1].distance) {
                    result.goodMatches.push_back(candidates[0]);
                }
            }
        }

        // Save current frame information for the next iteration.
        previousGray = result.gray;
        previousDescriptors = descriptors;
        previousKeypoints = result.keypoints;

        return result;
    }
};