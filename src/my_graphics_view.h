// Copyright (C) 2023 akiaji-k

#ifndef MY_GRAPHICS_VIEW_H
#define MY_GRAPHICS_VIEW_H

#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QPen>

#include <memory>
#include <optional>

#include "custom_types.h"

namespace Ui {
class MyGraphicsView;
}

class MyGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit MyGraphicsView(QWidget *parent = nullptr);
    ~MyGraphicsView();
    void ResetToInitial();

    void wheelEvent(QWheelEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void AddPixmapToScene(QPixmap pixmap);
    void SetCurrentProcess(Process pro)
    {
        m_current_process = pro;
        return;
    }

    bool HasIrradiatedRect() { return m_irradiated_rect.has_value(); }
    bool HasUnirradiatedRect() { return m_unirradiated_rect.has_value(); }
    bool HasNormalizeRect() { return m_normalize_rect.has_value(); }
    std::array<std::optional<QRectF>, 3> GetRoiRects();

private:
    /* declare parameters */
    static constexpr qreal ZOOM_SCALE = 0.05;
    static constexpr qreal ZOOM_IN_SCALE = 1.0 + ZOOM_SCALE;
    static constexpr qreal ZOOM_OUT_SCALE = 1.0 - ZOOM_SCALE;
    const QPen M_IRRADIATED_PEN = QPen(Qt::red, 3, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    const QPen M_UNIRRADIATED_PEN = QPen(Qt::blue, 3, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin);
    const QPen M_NORMALIZE_PEN = QPen(Qt::green, 3, Qt::DashDotDotLine, Qt::RoundCap, Qt::RoundJoin);

    /* declare variables */
    Ui::MyGraphicsView *m_ui;
    std::unique_ptr<QGraphicsScene> m_scene;
    Process m_current_process;

    QPoint m_last_translate_position;
    QPointF m_roi_start_position;
    std::optional<QGraphicsRectItem*> m_irradiated_rect;
    std::optional<QGraphicsRectItem*> m_unirradiated_rect;
    std::optional<QGraphicsRectItem*> m_normalize_rect;

signals:
    void roi_set_signal(const Process current_process);
};

#endif // MY_GRAPHICS_VIEW_H
