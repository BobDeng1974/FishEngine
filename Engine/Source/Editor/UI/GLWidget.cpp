#include <FishEngine/GLEnvironment.hpp>
#include "GLWidget.hpp"
#include <QMouseEvent>
#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>

#include <FishEngine/Debug.hpp>
#include <FishEngine/Screen.hpp>
#include <FishEngine/Input.hpp>
#include <FishEngine/RenderSystem.hpp>

#include "MainEditor.hpp"

using namespace FishEngine;
using namespace FishEditor;
//using namespace std;

Q_DECLARE_METATYPE(std::weak_ptr<FishEngine::Transform>);

GLWidget::GLWidget(QWidget *parent)
	: QOpenGLWidget(parent)
{
	setFocusPolicy(Qt::FocusPolicy::ClickFocus);
	//LogInfo("GLWidget::ctor");
	//Init();
	//auto filter = new EventFilter;
	//installEventFilter(filter);
}

GLWidget::~GLWidget()
{
	LogWarning("~GLWidget");
}

void GLWidget::initializeGL()
{
	initializeOpenGLFunctions();

	RenderSystem::InitializeGL();

	Screen::set(width(), height());
	MainEditor::Init();

	auto timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(1000 / 30.0f);
}

void GLWidget::paintGL()
{
	MainEditor::Run();
	
	Input::Update();

	auto globalCursorPos = QCursor::pos();
	auto localCursorPos = mapFromGlobal(globalCursorPos);
	// top-left -> bottom-left
	float x = localCursorPos.x() * Screen::pixelsPerPoint() / static_cast<float>(Screen::width());
	float y =  1.0f - localCursorPos.y() * Screen::pixelsPerPoint() / static_cast<float>(Screen::height());
	//Debug::Log("x = %lf, y = %lf", x, y);
	Input::UpdateMousePosition(x, y);
}


void GLWidget::resizeGL(int width, int height)
{
	LogInfo(Format( "resize w=%1% h =%2%", width, height ));
	int ratio = QApplication::desktop()->devicePixelRatio();
	LogInfo(Format("ratio = %1%", ratio ));
	Screen::setPixelsPerPoint(ratio);
	MainEditor::Resize(width, height);
	//m_mainSceneViewEditor->Resize(width*ratio, height*ratio);
}

//void GLWidget::mouseMoveEvent(QMouseEvent *event)
//{
//    auto p = event->localPos();
//    float x = p.x() / Screen::width();
//    float y = -p.y() / Screen::height();
//    //Debug::Log("x : %f, y: %f", x, y);
//    Input::UpdateMousePosition(x, y);
//}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
	int button = 0;
	auto b = event->button();
	if (b == Qt::LeftButton)
		button = 0;
	else if (b == Qt::RightButton)
		button = 1;
	else if (b == Qt::MiddleButton)
		button = 2;
	else
	{
		LogWarning("GLWidget::mousePressEvent: unknown mousebutton");
		return;
	}
	//Debug::LogWarning("mouse down: %d", button);
	Input::UpdateMouseButtonState(button, MouseButtonState::Down);
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
	int button = 0;
	auto b = event->button();
	if (b == Qt::LeftButton)
		button = 0;
	else if (b == Qt::RightButton)
		button = 1;
	else if (b == Qt::MiddleButton)
		button = 2;
	else
	{
		LogWarning("GLWidget::mousePressEvent: unknown mousebutton");
		return;
	}
	//Debug::LogWarning("mouse up: %d", button);
	Input::UpdateMouseButtonState(button, MouseButtonState::Up);
}


void GLWidget::wheelEvent(QWheelEvent *event)
{
	auto delta = 5.0f * event->angleDelta().y() / QWheelEvent::DefaultDeltasPerStep;
	//Debug::LogWarning("mouse scroll: %lf", delta);
	Input::UpdateAxis(Axis::MouseScrollWheel, delta);
}

//std::map<Qt::Key, FishEngine::KeyCode> keyMapping = {
//
//}

//std::map<int, int> keyMapping {
//	{Qt::Key_Alt, static_cast<int>(KeyCode::LeftAlt)},
//};

int KeyCodeFromQKeyEvent(QKeyEvent * event)
{
	int key = event->key();
	int virturlKey = event->nativeVirtualKey();
	int scanCode = event->nativeScanCode();
	//int modifiers = event->modifiers();
	//Debug::Log("key=%d virtualKey=%d scanCode=%d", key, virturlKey, scanCode);
	if (key >= Qt::Key_A && key <= Qt::Key_Z)
	{
		constexpr int offset = static_cast<int>(KeyCode::A) - Qt::Key_A;
		return key + offset;
	}
	else if (key >= Qt::Key_0 && key <= Qt::Key_9)
	{
		// TODO
		// keypad numbers

		//if (event->modifiers() & Qt::KeypadModifier)
		//{
		//	constexpr int offset = static_cast<int>(KeyCode::Keypad0) - Qt::Key_0;
		//	return key + offset;
		//}
		//else
		//{
			constexpr int offset = static_cast<int>(KeyCode::Alpha0) - Qt::Key_0;
			return key + offset;
		//}
	}
	else if (key >= Qt::Key_0 && key <= Qt::Key_9)
	{
		constexpr int offset = static_cast<int>(KeyCode::Alpha0) - Qt::Key_0;
		return key + offset;
	}
	else if (key >= Qt::Key_F1 && key <= Qt::Key_F15)
	{
		constexpr int offset = static_cast<int>(KeyCode::F1) - Qt::Key_F1;
		return key + offset;
	}
	
	return key;
}

void GLWidget::keyPressEvent(QKeyEvent * event)
{
	//FishEngine::Debug::LogWarning("Press");
	int key = KeyCodeFromQKeyEvent( event );
	Input::UpdateKeyState(key, KeyState::Down);
	int modifiers = event->modifiers();
	// TODO: left / right
	if (modifiers & Qt::KeyboardModifier::AltModifier)
	{
		//Debug::Log("Alt down");
		Input::UpdateKeyState(KeyCode::LeftAlt, KeyState::Down);
		Input::UpdateKeyState(KeyCode::RightAlt, KeyState::Down);
	}
	if (modifiers & Qt::KeyboardModifier::ControlModifier)
	{
		//Debug::Log("ctrl down");
		Input::UpdateKeyState(KeyCode::LeftControl, KeyState::Down);
		Input::UpdateKeyState(KeyCode::RightControl, KeyState::Down);
	}
	if (modifiers & Qt::KeyboardModifier::ShiftModifier)
	{
		//Debug::Log("shift down");
		Input::UpdateKeyState(KeyCode::LeftShift, KeyState::Down);
		Input::UpdateKeyState(KeyCode::RightShift, KeyState::Down);
	}
}

void GLWidget::keyReleaseEvent(QKeyEvent * event)
{
	//FishEngine::Debug::LogWarning("Release");
	int key = KeyCodeFromQKeyEvent(event);
	Input::UpdateKeyState(key, KeyState::Up);
	int modifiers = event->modifiers();
	key = event->key();
	if (key == Qt::Key_Alt)
	{
		//Debug::Log("Alt up");
		Input::UpdateKeyState(KeyCode::LeftAlt, KeyState::Up);
		Input::UpdateKeyState(KeyCode::RightAlt, KeyState::Up);
	}
	if (key == Qt::Key_Control)
	{
		//Debug::Log("ctrl up");
		Input::UpdateKeyState(KeyCode::LeftControl, KeyState::Up);
		Input::UpdateKeyState(KeyCode::RightControl, KeyState::Up);
	}
	if (key == Qt::Key_Shift)
	{
		//Debug::Log("shift up");
		Input::UpdateKeyState(KeyCode::LeftShift, KeyState::Up);
		Input::UpdateKeyState(KeyCode::RightShift, KeyState::Up);
	}
}
