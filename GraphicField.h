#ifndef GRAPHICFIELD_H
#define GRAPHICFIELD_H

#include <QWidget>
#include <QVector>
#include <QPainter>
#include <QResizeEvent>
#include "GameSystem.h"

namespace Ui {
class GraphicField;
}

class GraphicField : public QWidget
{
    Q_OBJECT
private:
    float image_part_width;
    float image_part_height;

    QImage field_resource[5];

protected:
    void resizeEvent(QResizeEvent * event); //リサイズイベント
    void paintEvent(QPaintEvent *);         //ペイントイベント
public:

    Field<GameSystem::MAP_OBJECT > field;
    Field<GameSystem::MAP_OVERLAY> overlay;

    explicit GraphicField(QWidget *parent = 0);
    ~GraphicField();

private:
    Ui::GraphicField *ui;

public slots:
};

#endif // GRAPHICFIELD_H
