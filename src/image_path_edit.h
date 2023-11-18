// Copyright (C) 2023 akiaji-k

#ifndef IMAGEPATHEDIT_H
#define IMAGEPATHEDIT_H

#include <QTextEdit>

#include <set>
#include <filesystem>
namespace fs = std::filesystem;

class ImagePathEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit ImagePathEdit(QWidget *parent = 0);
    void ResetToInitial();

    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;

    void DisplayInputs();
    bool PathValidation();
    void SetHowToUse();

private:
    QList<QUrl> m_input_urls;
    std::set<fs::path> m_image_extensions { ".tif", ".tiff"};
//    const QString m_how_to_use = tr("drag & drop input items or double-click to select input items.");

signals:
    void valid_image_path_signal(const QList<QUrl> &urls);
};

#endif // IMAGEPATHEDIT_H
