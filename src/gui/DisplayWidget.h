#ifndef DISPLAYWIDGET_H_NXVA5J03
#define DISPLAYWIDGET_H_NXVA5J03

#include <QOpenGLWidget>


#define FRAMEBUFFER_WIDTH 1008
#define FRAMEBUFFER_HEIGHT 256


class DisplayWidget: public QOpenGLWidget
{
Q_OBJECT
public:
	DisplayWidget(QWidget *parent = 0);
	virtual ~DisplayWidget();

protected:
	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int w, int h) override;

private:
	const uchar *framebuffer;
}; /* -----  end of class DisplayWidget  ----- */

#endif /* end of include guard: DISPLAYWIDGET_H_NXVA5J03 */
