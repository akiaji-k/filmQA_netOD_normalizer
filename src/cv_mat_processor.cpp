// Copyright (C) 2023 akiaji-k

#include "cv_mat_processor.h"

#include <QDebug>
#include <QRectF>

#include <utility>

#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

CvMatProcessor::CvMatProcessor()
    : m_averaged_mat(std::make_shared<cv::Mat>())
    ,m_netOD_mat(std::make_shared<cv::Mat>())
{}

void CvMatProcessor::ResetToInitial()
{
    qDebug() << "void CvMatProcessor::reset_to_initial() is called.";

    m_averaged_mat->release();
    m_netOD_mat->release();

    m_tiff_tags.clear();

    return;
}

std::optional<TiffTags> CvMatProcessor::ReadTiffTags(const std::string &input_file)
{
    qDebug() << "CvMatProcessor::read_tiff_tags(const fs::path &input_file) is called.";

    TiffTags tiff_tags;

    TIFF *tif = TIFFOpen(input_file.c_str(), "r");
    if(tif == nullptr) {
        return std::nullopt;
    }
    else { ; }

    if ( TIFFGetField( tif, TIFFTAG_IMAGEWIDTH, &tiff_tags.image_width) != 1 ) {
        return std::nullopt;
    }
    else if (TIFFGetField( tif, TIFFTAG_IMAGELENGTH, &tiff_tags.image_length) != 1) {
        return std::nullopt;
    }
    else if (TIFFGetField( tif, TIFFTAG_COMPRESSION, &tiff_tags.compression) != 1) {
        return std::nullopt;
    }
    else if (TIFFGetField( tif, TIFFTAG_XRESOLUTION, &tiff_tags.x_resolution) != 1) {
        return std::nullopt;
    }
    else if (TIFFGetField( tif, TIFFTAG_YRESOLUTION, &tiff_tags.y_resolution) != 1) {
        return std::nullopt;
    }
    else if (TIFFGetField( tif, TIFFTAG_RESOLUTIONUNIT, &tiff_tags.resolution_unit) != 1) {
        return std::nullopt;
    }
    else {
        return std::make_optional(tiff_tags);
    }
}

void CvMatProcessor::ShowTiffTags(const TiffTags &tags)
{
    qDebug() << "Image width:" << tags.image_width;
    qDebug() << "Image length:" << tags.image_length;
    qDebug() << "Compression:" << tags.compression;
    qDebug() << "X resolution:" << tags.x_resolution;
    qDebug() << "Y resolution:" << tags.y_resolution;
    qDebug() << "Resolution unit:" << tags.resolution_unit;

    return;
}

void CvMatProcessor::ShowCvMatInfo(const cv::Mat &mat)
{
    qDebug() << "show_cvmat_info() is called";
    qDebug() << "# of cols: " << mat.cols;
    qDebug() << "# of rows: " << mat.rows;
    qDebug() << "# of channels: " << mat.channels();
    qDebug() << "# of depth: " << mat.depth();
    qDebug() << "# of type: " << mat.type();

    auto depth = mat.depth();
    if (depth == CV_8U) { qDebug() << "CV_8U"; }
    else if (depth == CV_8S) { qDebug() << "CV_8S"; }
    else if (depth == CV_16U) { qDebug() << "CV_16U"; }
    else if (depth == CV_16S) { qDebug() << "CV_16S"; }
    else if (depth == CV_32F) { qDebug() << "CV_32F"; }
    else if (depth == CV_64F) { qDebug() << "CV_64F"; }
    else { qDebug() << "other depth."; }

    auto type = mat.type();
    if (type == CV_8UC3) { qDebug() << "CV_8UC3"; }
    else if (type == CV_8SC3) { qDebug() << "CV_8SC3"; }
    else if (type == CV_16UC1) { qDebug() << "CV_16UC1"; }
    else if (type == CV_16UC3) { qDebug() << "CV_16UC3"; }
    else if (type == CV_16SC3) { qDebug() << "CV_16SC3"; }
    else if (type == CV_32FC3) { qDebug() << "CV_32FC3"; }
    else if (type == CV_64FC3) { qDebug() << "CV_64FC3"; }
    else { qDebug() << "other type."; }

    return;
}

bool CvMatProcessor::IsValidRoiOnMat(const cv::Rect &rect, const cv::Mat &mat)
{
    if ((rect.x > 0.0)
            && (rect.y > 0.0)
            && (rect.width > 0.0)
            && (rect.height> 0.0)
            && (rect.x + rect.width <= mat.cols)
            && (rect.y + rect.height <= mat.rows)) {

        return true;
    }
    else {
        return false;
    }
}

cv::Scalar CvMatProcessor::GetMeanScalarInQrectfOnMat(const QRectF &rect, const cv::Mat &mat)
{
    cv::Rect cv_rect = cv::Rect(rect.x(), rect.y(), rect.width(), rect.height());
    cv::Mat cropped_mat = mat(cv_rect);

    return cv::mean(cropped_mat);
}

cv::Mat CvMatProcessor::GetCroppedMatByQrectfOnMat(const QRectF &rect, const cv::Mat &mat)
{
    cv::Rect cv_rect = cv::Rect(rect.x(), rect.y(), rect.width(), rect.height());

    return mat(cv_rect);
}

cv::Mat CvMatProcessor::CalculateNetODEquivalentMat(const cv::Mat &irradiated,
                                                    const cv::Mat &unirradiated)
{
    cv::Mat irradiated_mat = irradiated;
    cv::Mat netOD_mat(irradiated.rows, irradiated.cols, CV_64FC3);
    cv::Scalar mean_unirradiated = cv::mean(unirradiated);
    qDebug() << "unirradiated mean:" << cv::format(mean_unirradiated, cv::Formatter::FMT_DEFAULT);
    qDebug() << "B:" << mean_unirradiated.val[0] << "G:" << mean_unirradiated.val[1] << "R:" << mean_unirradiated.val[2] << "A:" << mean_unirradiated.val[3];

    irradiated_mat.convertTo(irradiated_mat, CV_64FC3, 1.0);
    cv::divide(irradiated_mat, mean_unirradiated, netOD_mat, 65535.0, CV_64FC3);
    netOD_mat.convertTo(netOD_mat, CV_16UC3, 1.0);
//    cv::imshow("netOD image", netOD_mat);

    return netOD_mat;
}

bool CvMatProcessor::WriteImages(const fs::path &output_dir,
                                 const std::array<std::optional<QRectF>, 2> &rects,
                                 const std::optional<cv::Scalar> &coef)
{
    qDebug() << "CvMatProcessor::write_images(const fs::path &output_dir, const std::array<std::optional<QRectF>, 4> &rects) is called.";

    /* calculate netOD */
    if(rects.at(0).has_value() && rects.at(1).has_value()) {
        qDebug() << "calculating netOD images.";
        auto rectf0 = rects.at(0).value();
        auto rectf1 = rects.at(1).value();

        cv::Rect rect0 = cv::Rect(rectf0.x(), rectf0.y(), rectf0.width(), rectf0.height());
        cv::Rect rect1 = cv::Rect(rectf1.x(), rectf1.y(), rectf1.width(), rectf1.height());

        if (IsValidRoiOnMat(rect0, *m_averaged_mat) && IsValidRoiOnMat(rect1, *m_averaged_mat)) {
            cv::Mat irradiated_image = (*m_averaged_mat)(rect0);
            cv::Mat unirradiated_image = (*m_averaged_mat)(rect1);
            qDebug() << "cropped image size in width:" << rect0.width << "and in height:" << rect0.height;
//            cv::imshow("cropped irradiated image", irradiated_image);
//            cv::imshow("cropped unirradiated image", unirradiated_image);

            /* create netOD equivalent image */
            *m_netOD_mat = CalculateNetODEquivalentMat(irradiated_image, unirradiated_image);
            //            cv::imshow("netOD image", *m_netOD_mat);
        } else {
            return false;
        }
    }
    else {
        return false;
    }

    /* normalization */
    if (coef.has_value()) {
        qDebug() << "normalization coefficients have values.";
        cv::multiply(*m_netOD_mat, coef.value(), *m_netOD_mat);
    }
    else {;}

    /* split BGR channels */
//            display_3ch_image(*m_netOD_mat);
    cv::Mat channels_bgr[3];
    cv::split(*m_netOD_mat, channels_bgr);
//            show_cvmat_info(channels_bgr[0]);

    /* write image */
    auto tags = m_tiff_tags.front().value();
    const std::vector<int> imwrite_flags {cv::IMWRITE_TIFF_RESUNIT, tags.resolution_unit,
                cv::IMWRITE_TIFF_XDPI, static_cast<int>(tags.x_resolution),
                cv::IMWRITE_TIFF_YDPI, static_cast<int>(tags.x_resolution),
                cv::IMWRITE_TIFF_COMPRESSION, tags.compression};

    fs::create_directories(output_dir);
    fs::path output_file_3ch = output_dir;
    output_file_3ch.append("3ch.tif");
    qDebug() << "output file:" << QString::fromStdString(output_file_3ch.string());
    cv::imwrite(output_file_3ch.string(), *m_netOD_mat, imwrite_flags);

    fs::path output_file_red = output_dir;
    output_file_red.append("red.tif");
    qDebug() << "output file:" << QString::fromStdString(output_file_red.string());
    cv::imwrite(output_file_red.string(), channels_bgr[2], imwrite_flags);

    fs::path output_file_green = output_dir;
    output_file_green.append("green.tif");
    qDebug() << "output file:" << QString::fromStdString(output_file_green.string());
    cv::imwrite(output_file_green.string(), channels_bgr[1], imwrite_flags);

    fs::path output_file_blue = output_dir;
    output_file_blue.append("blue.tif");
    qDebug() << "output file:" << QString::fromStdString(output_file_blue.string());
    cv::imwrite(output_file_blue.string(), channels_bgr[0], imwrite_flags);

    qDebug() << "output file done.";

    return true;
}

void CvMatProcessor::Display3chImage(const cv::Mat &mat)
{
    cv::Mat channels_bgr[3]; cv::split(mat, channels_bgr);
    cv::imshow("B of netOD image", channels_bgr[0]);
    cv::imshow("G of netOD image", channels_bgr[1]);
    cv::imshow("R of netOD image", channels_bgr[2]);

    return;
}

std::shared_ptr<cv::Mat> CvMatProcessor::OpenImages(const QList<QUrl> &urls)
{
    if (urls.size() < 1) {
        return nullptr;
    }

    /* read inputs */
    QList<cv::Mat> input_mats;
    for (auto&& elem : urls) {
//        input_mats.emplace_back(cv::imread(elem.toLocalFile().toStdString(), cv::IMREAD_UNCHANGED));
        input_mats.push_back(cv::imread(elem.toLocalFile().toStdString(), cv::IMREAD_UNCHANGED));		// for Qt5

        m_tiff_tags.emplace_back(ReadTiffTags(elem.toLocalFile().toStdString()));
        //        if (tiff_tags.back().has_value()) {
        //            show_tiff_tags(tiff_tags.back().value());
        //        }
    }
    qDebug() << "the # of input images:" << input_mats.size();
    ShowCvMatInfo(input_mats.front());

    for (auto&& elem : m_tiff_tags) {
        if (elem.has_value() == false) {
            return nullptr;
        }
    }
    for (size_t i = 0; i < (m_tiff_tags.size() - 1); ++i) {
        if ((m_tiff_tags.at(i).value().image_width != m_tiff_tags.at(i + 1).value().image_width)
                || (m_tiff_tags.at(i).value().image_length != m_tiff_tags.at(i + 1).value().image_length)
                || (m_tiff_tags.at(i).value().compression != m_tiff_tags.at(i + 1).value().compression)
                || (m_tiff_tags.at(i).value().x_resolution != m_tiff_tags.at(i + 1).value().x_resolution)
                || (m_tiff_tags.at(i).value().y_resolution != m_tiff_tags.at(i + 1).value().y_resolution)
                || (m_tiff_tags.at(i).value().resolution_unit != m_tiff_tags.at(i + 1).value().resolution_unit)
                ) {
            return nullptr;
        }
    }

    /* averaging */
    m_averaged_mat->create(input_mats.front().rows, input_mats.front().cols, CV_16UC3);

    if (input_mats.size() > 1) {
        qDebug() << "images are averaged...";
        cv::Mat averaged = cv::Mat::zeros(input_mats.at(0).rows, input_mats.at(0).cols, CV_64FC3);

        for (auto && elem : input_mats) {
//            show_cvmat_info(elem);
            elem.convertTo(elem, CV_64FC3);
//            show_cvmat_info(elem);

            averaged += elem;
        }

        averaged.convertTo(*m_averaged_mat, CV_16UC3, 1.0 / input_mats.size());
//        show_cvmat_info(*m_averaged_mat);
    }
    else {
        qDebug() << "image is not averaged...";
        *m_averaged_mat = input_mats.front();
//        show_cvmat_info(*m_averaged_mat);
    }

//    display_3ch_image(*m_averaged_mat);
    return m_averaged_mat;
}
