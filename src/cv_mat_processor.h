// Copyright (C) 2023 akiaji-k

#ifndef CVMATPROCESSOR_H
#define CVMATPROCESSOR_H

#include <QList>
#include <QObject>
#include <QUrl>

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>
namespace fs = std::filesystem;

#include "opencv2/opencv.hpp"
#include "tiffio.h"

#include "custom_types.h"

class CvMatProcessor : public QObject
{
    Q_OBJECT

public:
    CvMatProcessor();
    void ResetToInitial();

    std::optional<TiffTags> ReadTiffTags(const std::string &input_file);
    void ShowTiffTags(const TiffTags &tags);
    void ShowCvMatInfo(const cv::Mat &mat);
    bool IsValidRoiOnMat(const cv::Rect &rect, const cv::Mat &mat);
    cv::Scalar GetMeanScalarInQrectfOnMat(const QRectF &rect, const cv::Mat &mat);
    cv::Mat GetCroppedMatByQrectfOnMat(const QRectF &rect, const cv::Mat &mat);
    cv::Mat CalculateNetODEquivalentMat(const cv::Mat &irradiated, const cv::Mat &unirradiated);
    bool WriteImages(const fs::path &output_dir,
                     const std::array<std::optional<QRectF>, 2> &rects,
                     const std::optional<cv::Scalar> &coef);
    void Display3chImage(const cv::Mat &mat);

private:
    std::shared_ptr<cv::Mat> m_averaged_mat;
    std::shared_ptr<cv::Mat> m_netOD_mat;

    std::vector<std::optional<TiffTags>> m_tiff_tags;

public slots:
    std::shared_ptr<cv::Mat> OpenImages(const QList<QUrl> &urls);
};

#endif // CVMATPROCESSOR_H
