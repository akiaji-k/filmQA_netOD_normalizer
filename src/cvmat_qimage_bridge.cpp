// Copyright (C) 2023 akiaji-k

#include "cvmat_qimage_bridge.h"
#include <QDebug>

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"


CvMatQImageBridge::CvMatQImageBridge()
    : m_qimage(std::make_shared<QImage>())
{}

void CvMatQImageBridge::ResetToInitial()
{
    qDebug() << "void CvMatQImageBridge::reset_to_initial() is called.";

    m_qimage = nullptr;
    m_mat_8UC3.release();

    return;
}

std::shared_ptr<QImage> CvMatQImageBridge::CvMatToQImage(const cv::Mat &mat)
{
    qDebug() << "CvMatQImageBridge::cvmat_to_qimage(const cv::Mat &mat) is called.";

    m_mat_8UC3.create(mat.rows, mat.cols, CV_8UC3);

    // pixel value conversion below should switch depending on the input cv::Mat type.
    mat.convertTo(m_mat_8UC3, CV_8UC3, 1/255.0);
    cv::cvtColor(m_mat_8UC3, m_mat_8UC3, cv::COLOR_BGR2RGB);
//    show_cvmat_info(m_mat_8UC3);

    m_qimage = std::make_shared<QImage>((uchar*)m_mat_8UC3.data, m_mat_8UC3.cols, m_mat_8UC3.rows, static_cast<int>(m_mat_8UC3.step), QImage::Format_RGB888);


    return m_qimage;
}

void CvMatQImageBridge::CropImage(QPointF top_left, qreal width, qreal height)
{
    qDebug() << "CvMatQImageBridge::crop_image() is called.";
    if ((top_left.x() > 0.0)
            && (top_left.y() > 0.0)
            && (width > 0.0)
            && (height> 0.0)
            && (top_left.x() + width <= m_mat_8UC3.cols)
            && (top_left.y() + height <= m_mat_8UC3.rows)) {

        cv::Mat cropped_image = m_mat_8UC3(cv::Rect(top_left.x(), top_left.y(), width, height));
        cv::imshow("original image", m_mat_8UC3);
        cv::imshow("cropped image", cropped_image);
    }
    else {
        qDebug() << "ROI error";
    }

    return;
}


