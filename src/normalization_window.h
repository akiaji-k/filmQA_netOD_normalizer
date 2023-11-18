// Copyright (C) 2023 akiaji-k

#ifndef NORMALIZATION_WINDOW_H
#define NORMALIZATION_WINDOW_H

#include <QCheckBox>
#include <QWidget>

#include <memory>

#include "custom_types.h"
#include "cv_mat_processor.h"
#include "cvmat_qimage_bridge.h"
#include "image_path_edit.h"

namespace Ui {
class NormalizationWindow;
}

class NormalizationWindow : public QWidget
{
    Q_OBJECT

public:
    explicit NormalizationWindow(QWidget *parent = nullptr);
    ~NormalizationWindow();
    void ResetToInitial(bool called_from_owner);

    void InitConnections();
    void InitWindow();
    void ChangeProcess(const Process current_process);

private:
    Ui::NormalizationWindow *m_ui;
    std::shared_ptr<ImagePathEdit> m_image_path_edit;
    std::shared_ptr<QCheckBox> m_skip_unirradiated_check;

    std::unique_ptr<CvMatQImageBridge> m_mat_image_bridge;
    std::unique_ptr<CvMatProcessor> m_cv_mat_processor;

    // images
    std::shared_ptr<cv::Mat> m_averaged_mat;
    std::shared_ptr<QImage> m_qimg_buf;

    // control
    Process m_current_process = Process::ReadImages;
    bool m_image_is_read = false;

public slots:
    void ReceiveImagePathSlot(const QList<QUrl> &urls);
    void Process1ClickedSlot();
    void Process2ClickedSlot();
    void Process3ClickedSlot();
    void RoiSetSlot(const Process current_process);
    void SkipClickedSlot();

signals:
    void normalization_coefficient_signal(const cv::Scalar coef);
};

#endif // NORMALIZATION_WINDOW_H
