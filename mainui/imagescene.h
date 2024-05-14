#include <QGraphicsScene>
#include <QDebug>

class QGraphicsSceneMouseEvent;
class QKeyEvent;
class QGraphicsSceneContextMenuEvent;
class ImageScene : public QGraphicsScene
{
public:
	ImageScene(QObject* parent = nullptr);
	void mousePressEvent(QGraphicsSceneMouseEvent* event);
	void keyPressEvent(QKeyEvent* event) override;
	void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
};