#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <QOpenGLFunctions>
#include <QTimer>
#include <QWidget>
#include <QPainter>

#include "DisplayWidget.h"


DisplayWidget::DisplayWidget(QWidget *parent):
	QOpenGLWidget(parent),
	framebuffer(nullptr)
{
	int framebufferFd = shm_open("/simulator_fb", O_RDONLY, S_IRUSR | S_IWUSR);
	if (framebufferFd == -1) {
		qWarning("Framebuffer not opened");
	}
	else {
		framebuffer = static_cast<const uchar *>(mmap(NULL, FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT * 4, PROT_READ, MAP_SHARED, framebufferFd, 0));
		if (framebuffer == MAP_FAILED || framebuffer == nullptr) {
			qWarning("Framebuffer map failed");
		}
		::close(framebufferFd);
	}


	this->setFixedSize(FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);

	QTimer *timer = new QTimer(this);
	timer->setSingleShot(false);
	connect(timer, &QTimer::timeout, this, qOverload<>(&QWidget::update));
	if (format().swapInterval() == -1) {
		timer->setInterval(17);
	}
	else {
		timer->setInterval(0);
	}
	timer->start();
}

DisplayWidget::~DisplayWidget()
{
	if (framebuffer != MAP_FAILED && framebuffer != nullptr) {
		munmap(const_cast<void *>(static_cast<const void *>(framebuffer)), FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT * 4);
	}
}

void DisplayWidget::initializeGL()
{
	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
	f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void DisplayWidget::paintGL()
{
	if (framebuffer) {
		QImage img(framebuffer, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, QImage::Format_RGBA8888);
		QPainter p(this);
		p.drawImage(QPoint(0, 0), img);
	}
}

void DisplayWidget::resizeGL(int w, int h)
{
	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
	f->glViewport(0, 0, (GLint)w, (GLint)h);
}

