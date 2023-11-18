// Copyright (C) 2023 akiaji-k

#ifndef CVMATQIMAGEBRIDGE_H
#define CVMATQIMAGEBRIDGE_H

#include <QImage>
#include <QObject>
#include <memory>

#include "opencv2/core.hpp"

class CvMatQImageBridge : public QObject
{
    Q_OBJECT

public:
    CvMatQImageBridge();
    void ResetToInitial();

    std::shared_ptr<QImage> CvMatToQImage(const cv::Mat &mat);
    //    void ShowCvMatInfo(const cv::Mat &mat);

private:
    std::shared_ptr<QImage> m_qimage;
    cv::Mat m_mat_8UC3;

public slots:
    //    void ZoomImageSlot(QPoint step);
    void CropImage(QPointF top_left, qreal width, qreal height);
};

#endif // CVMATQIMAGEBRIDGE_H
