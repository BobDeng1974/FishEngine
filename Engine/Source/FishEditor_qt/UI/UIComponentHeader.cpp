#include "UIComponentHeader.hpp"
#include "ui_UIComponentHeader.h"

#include "UIDebug.hpp"

UIComponentHeader::UIComponentHeader(std::string const & componentTypeName, bool enabled, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::UIComponentHeader),
	m_enabled(enabled),
	m_componentTypeName(componentTypeName)
{
	ui->setupUi(this);

	ui->label->setText(QString(componentTypeName.c_str()));
	ui->checkBox->setChecked(m_enabled);

	connect(ui->checkBox, &QCheckBox::toggled, this, &UIComponentHeader::OnCheckBoxChanged);
	connect(ui->menuButton, &QPushButton::clicked, this, &UIComponentHeader::OnRemoveActionTrigered);
}

UIComponentHeader::UIComponentHeader(std::string const & componentTypeName, QWidget *parent) :
	UIComponentHeader(componentTypeName, true, parent)
{
	ui->checkBox->setHidden(true);
}

UIComponentHeader::~UIComponentHeader()
{
	delete ui;
}

FishEditor::UIHeaderState UIComponentHeader::CheckUpdate(const std::string &componentTypeName, bool &enabled)
{
	if (m_enabledChanged)
	{
		enabled = m_enabled;
		m_enabledChanged = false;
		return FishEditor::UIHeaderState::enabledChanged;
	}

	if (m_menuButtonTrigered)
	{
		m_menuButtonTrigered = false;
		return FishEditor::UIHeaderState::menuButtonClicked;
	}

	if (ui->checkBox->isHidden())
	{
		LOG;
		ui->checkBox->setHidden(false);
	}

	if (m_enabled != enabled)
	{
		LogInfo("[Header] new enabled");
		m_enabled = enabled;
		LOG;
		ui->checkBox->setChecked(m_enabled);
	}

	if (componentTypeName != m_componentTypeName)
	{
		LogInfo("[Header] new component name: " + componentTypeName);
		m_componentTypeName = componentTypeName;
		LOG;
		ui->label->setText(QString(componentTypeName.c_str()));
	}
	return FishEditor::UIHeaderState::none;
}

FishEditor::UIHeaderState UIComponentHeader::CheckUpdate(const std::string &componentTypeName)
{
	if (m_menuButtonTrigered)
	{
		m_menuButtonTrigered = false;
		return FishEditor::UIHeaderState::menuButtonClicked;
	}

	if (!ui->checkBox->isHidden())
	{
		LOG;
		ui->checkBox->setHidden(true);
	}

	if (componentTypeName != m_componentTypeName)
	{
		LogInfo("[Header] new component name: " + componentTypeName);
		m_componentTypeName = componentTypeName;
		LOG;
		ui->label->setText(QString(componentTypeName.c_str()));
	}
	return FishEditor::UIHeaderState::none;
}

void UIComponentHeader::OnCheckBoxChanged(bool value)
{
	m_enabled = value;
	m_enabledChanged = true;
}
