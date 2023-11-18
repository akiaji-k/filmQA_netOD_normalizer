// Copyright (C) 2023 akiaji-k

#include "my_graphics_view.h"
#include "ui_my_graphics_view.h"

#include <tuple>
#include <array>
#include <optional>

#include <QMouseEvent>
#include <QScrollBar>
#include <QGraphicsPixmapItem>
#include <QDebug>


MyGraphicsView::MyGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
    , m_ui(new Ui::MyGraphicsView)
    , m_scene(std::make_unique<QGraphicsScene>())
    , m_irradiated_rect(std::make_optional<QGraphicsRectItem*>())
    , m_unirradiated_rect(std::make_optional<QGraphicsRectItem*>())
    , m_normalize_rect(std::make_optional<QGraphicsRectItem*>())
{
    m_ui->setupUi(this);

    ResetToInitial();
}

MyGraphicsView::~MyGraphicsView()
{
    delete m_ui;
}

void MyGraphicsView::ResetToInitial()
{
    qDebug() << "void MyGraphicsView::reset_to_initial() is called.";

    m_scene->clear();
    m_current_process = Process::RoiIrradiated;

    m_irradiated_rect = std::nullopt;
    m_unirradiated_rect = std::nullopt;
    m_normalize_rect = std::nullopt;

    this->setScene(m_scene.get());

    return;
}


void MyGraphicsView::wheelEvent(QWheelEvent *event) {
//    qDebug() << "wheel event is called.";

    QPoint delta = event->angleDelta();
    if (!delta.isNull()) {
        if (delta.y() < 0) {
//            qDebug() << "zoom out";
            this->scale(ZOOM_OUT_SCALE, ZOOM_OUT_SCALE);
        }
        else {
//            qDebug() << "zoom in";
            this->scale(ZOOM_IN_SCALE, ZOOM_IN_SCALE);
        }
    }
    else { ; }

    return;
}


void MyGraphicsView::mousePressEvent(QMouseEvent *event) {
//    qDebug() << "mouse press evet is occured in MyGraphicsView.";

    if (event->buttons() == Qt::MiddleButton) {
//        m_last_translate_position = event->position().toPoint();
        m_last_translate_position = event->pos();		// for Qt5
    }
    else if (event->buttons() == Qt::LeftButton) {
//        QPoint pos = event->position().toPoint();
        QPoint pos = event->pos();		// for Qt5
//        qDebug() << "x: " << pos.x() << "y: " << pos.y();

        m_roi_start_position = mapToScene(pos);
//        qDebug() << "x: " << m_roi_start_position.x() << "y: " << m_roi_start_position.y();
    }
    else { ; }

    return;
}


void MyGraphicsView::mouseMoveEvent(QMouseEvent *event) {
//    qDebug() << "mouse move evet is occured.";

    if (event->buttons() == Qt::LeftButton) {
//        qDebug() << "Left button drag evet is occured.";

//        QPoint pos = event->position().toPoint();
        QPoint pos = event->pos();		// for Qt5
        QPointF roi_end_position = mapToScene(pos);

        QPointF top_left_pos = QPointF(std::min(m_roi_start_position.x(), roi_end_position.x()), std::min(m_roi_start_position.y(), roi_end_position.y()));
        QPointF bottom_right_pos = QPointF(std::max(m_roi_start_position.x(), roi_end_position.x()), std::max(m_roi_start_position.y(), roi_end_position.y()));
        qreal width = bottom_right_pos.x() - top_left_pos.x();
        qreal height = bottom_right_pos.y() - top_left_pos.y();
//        qDebug() << "top_left_pos:" << top_left_pos << "bottom_right_pos:" << bottom_right_pos << "width:" << width << "height:" << height;

        if (m_current_process == Process::RoiIrradiated) {
            if (m_irradiated_rect.has_value()) {
                m_scene->removeItem(m_irradiated_rect.value());
            }
            else {;}

            m_irradiated_rect = m_scene->addRect(top_left_pos.x(), top_left_pos.y(), width, height, M_IRRADIATED_PEN);
        }
        else if (m_current_process == Process::RoiUnIrradiated) {
            if (m_unirradiated_rect.has_value()) {
                m_scene->removeItem(m_unirradiated_rect.value());
            }
            else {;}

            m_unirradiated_rect = m_scene->addRect(top_left_pos.x(), top_left_pos.y(), width, height, M_UNIRRADIATED_PEN);
        }
        else if (m_current_process == Process::RoiNormalize) {
            if (m_normalize_rect.has_value()) {
                m_scene->removeItem(m_normalize_rect.value());
            }
            else {;}

            m_normalize_rect = m_scene->addRect(top_left_pos.x(), top_left_pos.y(), width, height, M_NORMALIZE_PEN);
        }
        else {;}
    }
    else if (event->buttons() == Qt::MiddleButton) {
//        qDebug() << "Middle button drag evet is occured.";

//        QPoint delta = event->position().toPoint() - m_last_translate_position;
        QPoint delta = event->pos() - m_last_translate_position;		// for Qt5
//        qDebug() << "delta:" << delta;
        this->horizontalScrollBar()->setValue( this->horizontalScrollBar()->value() - delta.x());
        this->verticalScrollBar()->setValue( this->verticalScrollBar()->value() - delta.y());
//        m_last_translate_position = event->position().toPoint();
        m_last_translate_position = event->pos();		// for Qt5
    }
    else { ; }


    return;
}


void MyGraphicsView::mouseReleaseEvent(QMouseEvent *event) {
//    qDebug() << "mouse release evet is occured in MyGraphicsView.";

    if (event->button() == Qt::LeftButton) {
//        emit crop_signal(m_irradiated_rect.value()->rect().topLeft(), m_irradiated_rect.value()->rect().width(), m_irradiated_rect.value()->rect().height());
        emit roi_set_signal(m_current_process);
    }
    else { ; }


    return;
}

void MyGraphicsView::AddPixmapToScene(QPixmap pixmap)
{
    m_scene->addPixmap(pixmap);
    return;
}

std::array<std::optional<QRectF>, 3> MyGraphicsView::GetRoiRects()
{
    std::optional<QRectF> rect1 = std::nullopt;
    std::optional<QRectF> rect2= std::nullopt;
    std::optional<QRectF> rect3= std::nullopt;

    if (m_irradiated_rect.has_value()) {rect1 = m_irradiated_rect.value()->rect();}
    if (m_unirradiated_rect.has_value()) {rect2 = m_unirradiated_rect.value()->rect();}
    if (m_normalize_rect.has_value()) {rect3 = m_normalize_rect.value()->rect();}

    return {rect1, rect2, rect3};
}
