// Copyright (C) 2023 akiaji-k

#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDebug>
#include <QPushButton>
#include <QSizePolicy>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
    , m_image_path_edit(std::make_shared<ImagePathEdit>())
    , m_normalization_grid(std::make_shared<QGridLayout>())
    , m_skip_normalize_check(std::make_shared<QCheckBox>(tr("Skip setting this ROI.")))
    , m_write_images_button(std::make_shared<PushButtonWithWordWrap>())
    , m_normalization_window_button(std::make_shared<QPushButton>())
    , m_normalization_label_1(std::make_shared<QLabel>())
    , m_normalization_label_2(std::make_shared<QLabel>())
    , m_mat_image_bridge(std::make_unique<CvMatQImageBridge>())
    , m_cv_mat_processor(std::make_unique<CvMatProcessor>())
    , m_normalization_window(std::make_unique<NormalizationWindow>())
    , m_document_view(std::make_unique<QWebEngineView>())
{
    m_ui->setupUi(this);
    InitWindow();
    InitConnections();

    m_current_process = Process::ReadImages;

    // for debug
//    QList<QUrl> urls {
//        QUrl("file:///C:/Users/thery/Desktop/tmp/20221126test/2hLater/DoseTable0/img20221126_16082436.tif"),
//        QUrl("file:///C:/Users/thery/Desktop/tmp/20221126test/2hLater/DoseTable0/img20221126_16072552.tif"),
//        QUrl("file:///C:/Users/thery/Desktop/tmp/20221126test/2hLater/DoseTable0/img20221126_16075495.tif")
//    };
//    receive_image_path_slot(urls);
    // end for debug

}


MainWindow::~MainWindow()
{
    delete m_ui;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    m_normalization_window->close();

    // if-statement below is left for future expandability.
    // (e.g. checking that whether "write images" is done or not might be need.)
    bool status = true;
    if (status) {
        event->accept();
    }
    else {
        event->ignore();
    }

    return;
}

void MainWindow::ResetToInitial()
{
    qDebug() << "void MainWindow::ResetToInitial() is called.";

    /* classes */
    m_ui->graphicsView->ResetToInitial();
    //    m_image_path_edit->ResetToInitial();		// not execute here because MainWindow::ResetToInitial() is called when m_image_path_edit emit the valid_image_path_signal().
    m_skip_normalize_check->setChecked(false);
    m_mat_image_bridge->ResetToInitial();
    m_cv_mat_processor->ResetToInitial();
    m_normalization_window->ResetToInitial(true);

    /* others */
    m_current_process = Process::ReadImages;
    m_image_is_read = false;
    m_averaged_mat = nullptr;
    m_qimg_buf = nullptr;
    m_output_dir.clear();
    m_normalization_coef = std::nullopt;
    m_normalization_label_1->setText("");
    m_normalization_label_2->setText("");

    return;
}

void MainWindow::InitConnections()
{
    connect(m_image_path_edit.get(),
            &ImagePathEdit::valid_image_path_signal,
            this,
            &MainWindow::ReceiveImagePathSlot);
    connect(m_ui->processStepWidget_1,
            &ProcessStepWidget::clicked_signal,
            this,
            &MainWindow::Process1ClickedSlot);
    connect(m_ui->processStepWidget_2,
            &ProcessStepWidget::clicked_signal,
            this,
            &MainWindow::Process2ClickedSlot);
    connect(m_ui->processStepWidget_3,
            &ProcessStepWidget::clicked_signal,
            this,
            &MainWindow::Process3ClickedSlot);
    connect(m_ui->processStepWidget_4,
            &ProcessStepWidget::clicked_signal,
            this,
            &MainWindow::Process4ClickedSlot);
    connect(m_ui->processStepWidget_5,
            &ProcessStepWidget::clicked_signal,
            this,
            &MainWindow::Process5ClickedSlot);
    connect(m_ui->graphicsView, &MyGraphicsView::roi_set_signal, this, &MainWindow::RoiSetSlot);
    connect(m_skip_normalize_check.get(),
            &QAbstractButton::clicked,
            this,
            &MainWindow::Process5ClickedSlot);
    connect(m_normalization_window_button.get(), &QAbstractButton::clicked, m_normalization_window.get(), &QWidget::show);
    connect(m_normalization_window.get(),
            &NormalizationWindow::normalization_coefficient_signal,
            this,
            &MainWindow::NormalizationCoefficientSlot);

    connect(m_write_images_button.get(),
            &QAbstractButton::clicked,
            this,
            &MainWindow::WriteImageSlot);

    connect(m_ui->actionShow_Document, &QAction::triggered, this, &MainWindow::ShowDocument);

    return;
}

void MainWindow::InitWindow()
{
    bool wait_status = false;
//    bool wait_status = true;

    // step 1: Select input images.
    static_cast<QGridLayout*>(m_ui->processStepWidget_1->layout())->addWidget(m_image_path_edit.get(), 1, 1);
    m_ui->processStepWidget_1->SetIndex(1);
    m_ui->processStepWidget_1->SetExplanation(tr("Select input images."));
    m_ui->processStepWidget_1->SetBorderStyle("solid");
    m_ui->processStepWidget_1->SetBorderColor("black");
    m_ui->processStepWidget_1->Activate(true);

    // step 2: Set ROI on the irradiated film.
    m_ui->processStepWidget_2->SetIndex(2);
    m_ui->processStepWidget_2->SetExplanation(tr("Set ROI on the irradiated film."));
    m_ui->processStepWidget_2->SetBorderStyle("dashed");
    m_ui->processStepWidget_2->SetBorderColor("red");
    m_ui->processStepWidget_2->Activate(wait_status);

    // step 3: Set ROI on the unirradiated film.
    m_ui->processStepWidget_3->SetIndex(3);
    m_ui->processStepWidget_3->SetExplanation(tr("Set ROI on the unirradiated film."));
    m_ui->processStepWidget_3->SetBorderStyle("dotted");
    m_ui->processStepWidget_3->SetBorderColor("blue");
    m_ui->processStepWidget_3->Activate(wait_status);

    // step 4: Set ROI on the normalization film.
    m_normalization_window_button->setText(tr("Set Reference."));
    m_normalization_label_1->setText("");
    m_normalization_label_2->setText("");
    m_normalization_label_1->setStyleSheet("font-size: 12px;");
    m_normalization_label_2->setStyleSheet("font-size: 16px; text-decoration: underline;");
    m_normalization_grid->addWidget(m_skip_normalize_check.get(), 0, 0);
    m_normalization_grid->addWidget(m_normalization_window_button.get(), 0, 2);
    m_normalization_grid->addWidget(m_normalization_label_1.get(), 1, 0, 1, -1);	// -1 means full-width of this row
    m_normalization_grid->addWidget(m_normalization_label_2.get(), 2, 0, 1, -1);	// -1 means full-width of this row
    m_normalization_grid->setColumnStretch(0, 5);
    m_normalization_grid->setColumnStretch(1, 3);
    m_normalization_grid->setColumnStretch(2, 5);
    m_normalization_grid->setRowStretch(0, 1);
    m_normalization_grid->setRowStretch(1, 1);
    m_normalization_grid->setRowStretch(2, 1);
    static_cast<QGridLayout*>(m_ui->processStepWidget_4->layout())->addLayout(m_normalization_grid.get(), 1, 1);

    static_cast<QGridLayout*>(m_ui->processStepWidget_4->layout())->setColumnStretch(0, 1);
    static_cast<QGridLayout*>(m_ui->processStepWidget_4->layout())->setColumnStretch(1, 8);
    static_cast<QGridLayout*>(m_ui->processStepWidget_4->layout())->setColumnStretch(2, 1);
    m_skip_normalize_check->setStyleSheet("font-size: 20px;");
    m_ui->processStepWidget_4->SetIndex(4);
    m_ui->processStepWidget_4->SetExplanation(tr("Set ROI on the normalization film."));
    m_ui->processStepWidget_4->SetBorderStyle("dot-dot-dash");
    m_ui->processStepWidget_4->SetBorderColor("green");
    m_ui->processStepWidget_4->Activate(wait_status);

    // step 5: Write images.
    m_write_images_button->SetText(tr("Write to: "));
    m_write_images_button->setStyleSheet("Text-align: left");
    static_cast<QGridLayout*>(m_ui->processStepWidget_5->layout())->addWidget(m_write_images_button.get(), 1, 1);
    m_ui->processStepWidget_5->SetIndex(5);
    m_ui->processStepWidget_5->SetExplanation(tr("Write images."));
    m_ui->processStepWidget_5->SetBorderStyle("solid");
    m_ui->processStepWidget_5->SetBorderColor("black");
    m_ui->processStepWidget_5->Activate(wait_status);

    return;
}

void MainWindow::ChangeProcess(const Process current_process)
{
    if (m_current_process == Process::ReadImages) {
        m_ui->processStepWidget_1->Activate(true);
        m_ui->processStepWidget_2->Activate(false);
        m_ui->processStepWidget_3->Activate(false);
        m_ui->processStepWidget_4->Activate(false);
        m_ui->processStepWidget_5->Activate(false);
    }
    else if (m_current_process == Process::RoiIrradiated) {
        m_ui->processStepWidget_1->Activate(false);
        m_ui->processStepWidget_2->Activate(true);
        m_ui->processStepWidget_3->Activate(false);
        m_ui->processStepWidget_4->Activate(false);
        m_ui->processStepWidget_5->Activate(false);
    }
    else if (m_current_process == Process::RoiUnIrradiated) {
        m_ui->processStepWidget_1->Activate(false);
        m_ui->processStepWidget_2->Activate(false);
        m_ui->processStepWidget_3->Activate(true);
        m_ui->processStepWidget_4->Activate(false);
        m_ui->processStepWidget_5->Activate(false);
    }
    else if (m_current_process == Process::RoiNormalize) {
        m_ui->processStepWidget_1->Activate(false);
        m_ui->processStepWidget_2->Activate(false);
        m_ui->processStepWidget_3->Activate(false);
        m_ui->processStepWidget_4->Activate(true);
        m_ui->processStepWidget_5->Activate(false);
    }
    else if (m_current_process == Process::WriteImages) {
        m_ui->processStepWidget_1->Activate(false);
        m_ui->processStepWidget_2->Activate(false);
        m_ui->processStepWidget_3->Activate(false);
        m_ui->processStepWidget_4->Activate(false);
        m_ui->processStepWidget_5->Activate(true);
    }
    else {
        m_ui->processStepWidget_1->Activate(true);
        m_ui->processStepWidget_2->Activate(false);
        m_ui->processStepWidget_3->Activate(false);
        m_ui->processStepWidget_4->Activate(false);
        m_ui->processStepWidget_5->Activate(false);
    }

    m_ui->graphicsView->SetCurrentProcess(m_current_process);

    return;
}

void MainWindow::ReceiveImagePathSlot(const QList<QUrl> &urls)
{
    ResetToInitial();

    /* get shared pointer of the averaged image */
    m_averaged_mat = m_cv_mat_processor->OpenImages(urls);

    /* display it */
    m_qimg_buf = m_mat_image_bridge->CvMatToQImage(*m_averaged_mat);
    m_ui->graphicsView->AddPixmapToScene(QPixmap::fromImage(*m_qimg_buf));

    m_current_process = Process::RoiIrradiated;
    ChangeProcess(m_current_process);

    /* set path to output directory */
    m_output_dir = fs::path(urls.front().toLocalFile().toStdString()).parent_path();
    m_write_images_button->SetText(
        QString(tr("Write to: %1")).arg(QString::fromStdString(m_output_dir.generic_string())));
    m_output_dir.append("outputs");
    qDebug() << "output dir: " << QString::fromStdString(m_output_dir.generic_string());

    m_image_is_read = true;

    return;
}

void MainWindow::Process1ClickedSlot()
{
    m_current_process = Process::ReadImages;
    ChangeProcess(m_current_process);
    return;
}

void MainWindow::Process2ClickedSlot()
{
    if (m_image_is_read) {
        m_current_process = Process::RoiIrradiated;
        ChangeProcess(m_current_process);
    }
    else {;}

    return;
}

void MainWindow::Process3ClickedSlot()
{
    if (m_ui->graphicsView->HasIrradiatedRect()) {
        m_current_process = Process::RoiUnIrradiated;
        ChangeProcess(m_current_process);
    } else {
        ;
    }

    return;
}

void MainWindow::Process4ClickedSlot()
{
    if ((m_ui->graphicsView->HasIrradiatedRect()) && (m_ui->graphicsView->HasUnirradiatedRect())) {
        m_current_process = Process::RoiNormalize;
        ChangeProcess(m_current_process);
    } else {
        ;
    }

    return;
}

void MainWindow::Process5ClickedSlot()
{
    if ((m_ui->graphicsView->HasIrradiatedRect()) && (m_ui->graphicsView->HasUnirradiatedRect())
        && ((m_normalization_coef.has_value()) || (m_skip_normalize_check->isChecked()))) {
        m_current_process = Process::WriteImages;
        ChangeProcess(m_current_process);
    } else {
        ;
    }

    return;
}

void MainWindow::RoiSetSlot(const Process current_process)
{
    if (current_process == Process::RoiIrradiated) {
        m_current_process = Process::RoiUnIrradiated;
        ChangeProcess(m_current_process);
    }
    else if (current_process == Process::RoiUnIrradiated) {
        m_current_process = Process::RoiNormalize;
        ChangeProcess(m_current_process);
    }
    else if (current_process == Process::RoiNormalize) {
        m_normalization_window->show();
        //        m_current_process = Process::WriteImages;
        //        ChangeProcess(m_current_process);
    }
    else {;}

    return;
}

void MainWindow::NormalizationCoefficientSlot(const cv::Scalar coef)
{
    qDebug() << "void MainWindow::normalization_coefficient_slot(const cv::Scalar coef) is called.";

    qDebug() << "reference mean B:" << coef[0] << "G:" << coef[1] << "R:" << coef[2];

    /* calculate normalization coefficients */
    if ((m_ui->graphicsView->HasUnirradiatedRect()) && (m_ui->graphicsView->HasNormalizeRect())) {
        auto three_rects = m_ui->graphicsView->GetRoiRects();

        auto netOD_mat = m_cv_mat_processor->CalculateNetODEquivalentMat(
            *m_averaged_mat,
            m_cv_mat_processor->GetCroppedMatByQrectfOnMat(three_rects.at(1).value(),
                                                           *m_averaged_mat));
        auto mean_scalar = m_cv_mat_processor->GetMeanScalarInQrectfOnMat(three_rects.at(2).value(),
                                                                          netOD_mat);
        qDebug() << "current mean B:" << mean_scalar[0] << "G:" << mean_scalar[1] << "R:" << mean_scalar[2];

        cv::Scalar normalization_coef;
        cv::divide(mean_scalar, coef, normalization_coef);
        m_normalization_coef = std::make_optional(normalization_coef);
        qDebug() << "normalize coefficients B:" << mean_scalar[0] / coef[0] << "G:" << mean_scalar[1] / coef[1]<< "R:" << mean_scalar[2] / coef[2];
        qDebug() << "normalize coefficients B:" << m_normalization_coef.value()[0] << "G:" << m_normalization_coef.value()[1]<< "R:" << m_normalization_coef.value()[2];

        QString means = QString(tr("current mean RGB(%1, %2, %3)\n")).arg(mean_scalar[2]).arg(mean_scalar[1]).arg(mean_scalar[0]);
        means += QString(tr("reference mean RGB(%1, %2, %3)")).arg(coef[2]).arg(coef[1]).arg(coef[0]);
        m_normalization_label_1->setText(means);

        QString ratio = QString(tr("normalization coef. RGB(%1, %2, %3)\n")).arg(m_normalization_coef.value()[2]).arg(m_normalization_coef.value()[1]).arg(m_normalization_coef.value()[0]);
        m_normalization_label_2->setText(ratio);

        // set current process to WriteImages
        m_current_process = Process::WriteImages;
        ChangeProcess(m_current_process);
        m_normalization_window->hide();
    } else {
        m_normalization_label_1->setText("");
        m_normalization_label_2->setText("");
    }

    return;
}

void MainWindow::WriteImageSlot()
{
    auto three_rects = m_ui->graphicsView->GetRoiRects();

    std::array<std::optional<QRectF>, 2> roi_rects {three_rects.at(0), three_rects.at(1)};

    fs::path output_dir = m_output_dir;
    if (m_normalization_coef.has_value()) {
        output_dir.concat("_w_normalization");
    }
    else {
        output_dir.concat("_wo_normalization");
    }

    if (m_cv_mat_processor->WriteImages(output_dir, roi_rects, m_normalization_coef) == false) {
        QMessageBox::warning(this, tr(""),
                             tr("Images are not written.\nROIs might be invalid. \nSet valid ROIs."),
                             QMessageBox::Ok,
                             QMessageBox::Ok
                             );
    } else {
        QMessageBox::information(this, tr(""),
                             tr("Images are written successfully."),
                             QMessageBox::Ok,
                             QMessageBox::Ok
                             );
        ;
    }

    return;
}

void MainWindow::ShowDocument()
{
    qDebug() << "void MainWindow::show_document() is called.";
    m_document_view->load(QUrl("C:/Users/thery/qt_sources/filmQA_netOD_normalizer_v1/README.html"));
    m_document_view->show();

    return;
}
