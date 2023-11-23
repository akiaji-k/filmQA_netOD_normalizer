// Copyright (C) 2023 akiaji-k

#include "normalization_window.h"
#include "ui_normalization_window.h"

#include <QGridLayout>
#include <QDebug>

NormalizationWindow::NormalizationWindow(QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui::NormalizationWindow)
    , m_image_path_edit(std::make_shared<ImagePathEdit>())
    , m_skip_unirradiated_check(
          std::make_shared<QCheckBox>(tr("Skip setting this ROI since the inputs are \nnetOD "
                                         "equivalent (rescaled to 16-bit) images.")))
    , m_mat_image_bridge(std::make_unique<CvMatQImageBridge>())
    , m_cv_mat_processor(std::make_unique<CvMatProcessor>())
{
    m_ui->setupUi(this);

    InitWindow();
    InitConnections();
}

NormalizationWindow::~NormalizationWindow()
{
    delete m_ui;
}

void NormalizationWindow::ResetToInitial(bool called_from_owner)
{
    qDebug() << "void NormalizationWindow::ResetToInitial() is called.";

    /* classes */
    m_ui->graphicsView->ResetToInitial();
    if (called_from_owner) {
        //	    m_image_path_edit->ResetToInitial();		// not execute here because NormalizationWindow::ResetToInitial() is called when m_image_path_edit emit the valid_image_path_signal().
        m_image_path_edit->SetHowToUse();
    } else {
        ;
    }
    m_skip_unirradiated_check->setChecked(false);
    m_mat_image_bridge->ResetToInitial();
    m_cv_mat_processor->ResetToInitial();

    /* others */
    m_current_process = Process::ReadImages;
    ChangeProcess(m_current_process);
    m_image_is_read = false;
    m_averaged_mat = nullptr;
    m_qimg_buf = nullptr;

    return;
}

void NormalizationWindow::InitConnections()
{
    connect(m_image_path_edit.get(),
            &ImagePathEdit::valid_image_path_signal,
            this,
            &NormalizationWindow::ReceiveImagePathSlot);

    connect(m_ui->processStepWidget_1,
            &ProcessStepWidget::clicked_signal,
            this,
            &NormalizationWindow::Process1ClickedSlot);
    connect(m_ui->processStepWidget_2,
            &ProcessStepWidget::clicked_signal,
            this,
            &NormalizationWindow::Process2ClickedSlot);
    connect(m_ui->processStepWidget_3,
            &ProcessStepWidget::clicked_signal,
            this,
            &NormalizationWindow::Process3ClickedSlot);
    connect(m_ui->graphicsView,
            &MyGraphicsView::roi_set_signal,
            this,
            &NormalizationWindow::RoiSetSlot);
    connect(m_skip_unirradiated_check.get(),
            &QAbstractButton::clicked,
            this,
            &NormalizationWindow::SkipClickedSlot);

    return;
}

void NormalizationWindow::InitWindow()
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

    // step 2: Set ROI on the normalization reference film.
    m_ui->processStepWidget_2->SetIndex(2);
    m_ui->processStepWidget_2->SetExplanation(tr("Set ROI on the normalization film."));
    m_ui->processStepWidget_2->SetBorderStyle("dot-dot-dash");
    m_ui->processStepWidget_2->SetBorderColor("green");
    m_ui->processStepWidget_2->Activate(wait_status);

    // step 3: Set ROI on the unirradiated film.
    m_ui->processStepWidget_3->SetIndex(3);
    m_ui->processStepWidget_3->SetExplanation(tr("Set ROI on the unirradiated film."));
    //    m_skip_unirradiated_check->setStyleSheet("font-size: 20px;");
    m_skip_unirradiated_check->setStyleSheet("font-size: 16px;");
    static_cast<QGridLayout*>(m_ui->processStepWidget_3->layout())->addWidget(m_skip_unirradiated_check.get(), 1, 1);
    m_ui->processStepWidget_3->SetBorderStyle("dotted");
    m_ui->processStepWidget_3->SetBorderColor("blue");
    m_ui->processStepWidget_3->Activate(wait_status);

    return;
}

void NormalizationWindow::ChangeProcess(const Process current_process)
{
    if (m_current_process == Process::ReadImages) {
        m_ui->processStepWidget_1->Activate(true);
        m_ui->processStepWidget_2->Activate(false);
        m_ui->processStepWidget_3->Activate(false);
    }
    else if (m_current_process == Process::RoiNormalize) {
        m_ui->processStepWidget_1->Activate(false);
        m_ui->processStepWidget_2->Activate(true);
        m_ui->processStepWidget_3->Activate(false);
    }
    else if (m_current_process == Process::RoiUnIrradiated) {
        m_ui->processStepWidget_1->Activate(false);
        m_ui->processStepWidget_2->Activate(false);
        m_ui->processStepWidget_3->Activate(true);
    }
    else {
        m_ui->processStepWidget_1->Activate(true);
        m_ui->processStepWidget_2->Activate(false);
        m_ui->processStepWidget_3->Activate(false);
    }

    m_ui->graphicsView->SetCurrentProcess(m_current_process);

    return;
}

void NormalizationWindow::ReceiveImagePathSlot(const QList<QUrl> &urls)
{
    ResetToInitial(false);

    /* get shared pointer of the averaged image */
    m_averaged_mat = m_cv_mat_processor->OpenImages(urls);

    /* display it */
    m_qimg_buf = m_mat_image_bridge->CvMatToQImage(*m_averaged_mat);
    m_ui->graphicsView->AddPixmapToScene(QPixmap::fromImage(*m_qimg_buf));

    m_current_process = Process::RoiNormalize;
    ChangeProcess(m_current_process);

    m_image_is_read = true;

    return;
}

void NormalizationWindow::Process1ClickedSlot()
{
    m_current_process = Process::ReadImages;
    ChangeProcess(m_current_process);
    return;
}

void NormalizationWindow::Process2ClickedSlot()
{
    if (m_image_is_read) {
        m_current_process = Process::RoiNormalize;
        ChangeProcess(m_current_process);
    }
    else {;}

    return;
}

void NormalizationWindow::Process3ClickedSlot()
{
    if (m_ui->graphicsView->HasNormalizeRect()) {
        m_current_process = Process::RoiUnIrradiated;
        ChangeProcess(m_current_process);
    } else {
        ;
    }

    return;
}

void NormalizationWindow::RoiSetSlot(const Process current_process)
{
    if (current_process == Process::RoiNormalize) {
        m_current_process = Process::RoiUnIrradiated;
        ChangeProcess(m_current_process);
    }
    else if (current_process == Process::RoiUnIrradiated) {
        auto three_rects = m_ui->graphicsView->GetRoiRects();
        if (three_rects.at(2).has_value() && three_rects.at(1).has_value()) {	// normalization rect & unirradiated rect
            auto netOD_mat = m_cv_mat_processor->CalculateNetODEquivalentMat(
                *m_averaged_mat,
                m_cv_mat_processor->GetCroppedMatByQrectfOnMat(three_rects.at(1).value(),
                                                               *m_averaged_mat));
            auto mean_scalar = m_cv_mat_processor
                                   ->GetMeanScalarInQrectfOnMat(three_rects.at(2).value(),
                                                                netOD_mat);
            emit normalization_coefficient_signal(mean_scalar);
        }
    }
    else {;}

    return;
}

void NormalizationWindow::SkipClickedSlot()
{
    if (m_skip_unirradiated_check->isChecked()) {
        auto three_rects = m_ui->graphicsView->GetRoiRects();
        if (three_rects.at(2).has_value()) {	// normalization rect
            auto mean_scalar = m_cv_mat_processor
                                   ->GetMeanScalarInQrectfOnMat(three_rects.at(2).value(),
                                                                *m_averaged_mat);
            emit normalization_coefficient_signal(mean_scalar);
        }
    }
    else { ; }
}




