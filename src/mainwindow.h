// Copyright (C) 2023 akiaji-k

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCheckBox>
#include <QGraphicsScene>
#include <QGridLayout>
#include <QImage>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QWebEngineView>

#include <filesystem>
#include <memory>
namespace fs = std::filesystem;

#include "custom_types.h"
#include "cvmat_qimage_bridge.h"
#include "image_path_edit.h"
#include "cv_mat_processor.h"
#include "normalization_window.h"
#include "pushbutton_with_word_wrap.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *m_ui;
    std::shared_ptr<ImagePathEdit> m_image_path_edit;
    std::shared_ptr<QGridLayout> m_normalization_grid;
    std::shared_ptr<QCheckBox> m_skip_normalize_check;
    std::shared_ptr<PushButtonWithWordWrap> m_write_images_button;
    std::shared_ptr<QPushButton> m_normalization_window_button;
    std::shared_ptr<QLabel> m_normalization_label_1;
    std::shared_ptr<QLabel> m_normalization_label_2;

    std::unique_ptr<CvMatQImageBridge> m_mat_image_bridge;
    std::unique_ptr<CvMatProcessor> m_cv_mat_processor;
    std::unique_ptr<NormalizationWindow> m_normalization_window;

    // images
    std::shared_ptr<cv::Mat> m_averaged_mat;
    std::shared_ptr<QImage> m_qimg_buf;
    fs::path m_output_dir;
    std::optional<cv::Scalar> m_normalization_coef = std::nullopt;

    // control
    Process m_current_process = Process::ReadImages;
    bool m_image_is_read = false;
    std::unique_ptr<QWebEngineView> m_document_view;

    // functions
    void closeEvent(QCloseEvent *event) override;
    void ResetToInitial();
    void InitConnections();
    void InitWindow();
    void ChangeProcess(const Process current_process);
    void ShowDocument();

public slots:
    void ReceiveImagePathSlot(const QList<QUrl> &urls);
    void Process1ClickedSlot();
    void Process2ClickedSlot();
    void Process3ClickedSlot();
    void Process4ClickedSlot();
    void Process5ClickedSlot();
    void RoiSetSlot(const Process current_process);
    void NormalizationCoefficientSlot(const cv::Scalar coef);
    void WriteImageSlot();
};
#endif // MAINWINDOW_H
