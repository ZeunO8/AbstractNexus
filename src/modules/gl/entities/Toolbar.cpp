#include <anex/modules/gl/entities/Toolbar.hpp>
#include <iostream>
using namespace anex::modules::gl::entities;
Toolbar::Toolbar(GLWindow &window,
                GLScene &scene,
                 const glm::vec3 &position,
                 const glm::vec3 &rotation,
                 const glm::vec3 &scale,
                 const glm::vec4 &color,
                 const float &height,
								 fonts::freetype::FreetypeFont &font):
	GLEntity(window, {
			"Color", "Position",
			"View", "Projection", "Model", "CameraPosition"
	}, 6,
	{
		0, 1, 2,  2, 3, 0   // Front face
	}, 4,
	{
		{ 0, -height, 0}, { window.windowWidth, -height, 0}, { window.windowWidth, 0, 0 }, { 0, 0, 0} // Front
	},
	position, rotation, scale),
	colors({color, color, color, color}),
	scene(scene),
	font(font),
	height(height)
{
	updateIndices(indices);
	updateElements("Color", colors);
	updateElements("Position", positions);
	//
	float toolButtonsX = window.windowWidth;
	// X Button
  xButton = std::make_shared<Plane>(window, scene, glm::vec3(toolButtonsX = (toolButtonsX - (height / 2)), -height / 2, 0.5), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), glm::vec2(height, height), glm::vec4(1, 0, 0, 1));
  addChild(xButton);
  xString = "X";
  float xFontSize = height;
  float xLineHeight = 0;
	auto xTextSize = font.stringSize(xString, xFontSize, xLineHeight, {0, 0});
  xTextView = std::make_shared<TextView>(window, scene, glm::vec3(0, 0, 0.5f), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), xString, xTextSize / 2.f, font, xFontSize);
  xButton->addChild(xTextView);
	xTextView->addToBVH = false;
	xButtonLeftMousePressID = xButton->addMousePressHandler(0, [&](auto &pressed)
	{
		window.close();
	});
	xButtonMouseHoverID = xButton->addMouseHoverHandler([&](auto &hovered)
	{
		if (hovered)
		{
			xButton->setColor({0.8, 0, 0, 1});
		}
		else
		{
			xButton->setColor({1, 0, 0, 1});
		}
	});
	// max Button
	maxButton = std::make_shared<Plane>(window, scene, glm::vec3(toolButtonsX = (toolButtonsX - (xButton->size.x)), -height /2, 0.5), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), glm::vec2(height, height), glm::vec4(0.4f, 0.4f, 0.4f, 1));
	addChild(maxButton);
	maxString = "+";
	float maxFontSize = height;
	float maxLineHeight = 0;
	auto maxTextSize = font.stringSize(maxString, maxFontSize, maxLineHeight, {0, 0});
	maxTextView = std::make_shared<TextView>(window, scene, glm::vec3(0, 0, 0.5f), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), maxString, maxTextSize / 2.f, font, maxFontSize);
	maxButton->addChild(maxTextView);
	maxTextView->addToBVH = false;
	maxButtonLeftMousePressID = maxButton->addMousePressHandler(0, [&](auto &pressed)
	{
		window.maximize();
	});
	maxButtonMouseHoverID = maxButton->addMouseHoverHandler([&](auto &hovered)
	{
		if (hovered)
		{
			maxButton->setColor({0.2, 0.2, 0.2, 1});
		}
		else
		{
			maxButton->setColor({0.4, 0.4, 0.4, 1});
		}
	});
	// _ Button
	_Button = std::make_shared<Plane>(window, scene, glm::vec3(toolButtonsX = (toolButtonsX - (maxButton->size.x)), -height /2, 0.5), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), glm::vec2(height, height), glm::vec4(0.4f, 0.4f, 0.4f, 1));
	addChild(_Button);
	_String = "_";
	float _FontSize = height;
	float _LineHeight = 0;
	auto _TextSize = font.stringSize(_String, _FontSize, _LineHeight, {0, 0});
	_TextView = std::make_shared<TextView>(window, scene, glm::vec3(0, 0, 0.5f), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), _String, _TextSize / 2.f, font, _FontSize);
	_Button->addChild(_TextView);
	_TextView->addToBVH = false;
	_ButtonLeftMousePressID = _Button->addMousePressHandler(0, [&](auto &pressed)
	{
		window.minimize();
	});
	_ButtonMouseHoverID = _Button->addMouseHoverHandler([&](auto &hovered)
	{
		if (hovered)
		{
			_Button->setColor({0.2, 0.2, 0.2, 1});
		}
		else
		{
			_Button->setColor({0.4, 0.4, 0.4, 1});
		}
	});
	// icon
	iconTexture.reset(new textures::Texture(window, {128, 128, 1, 0}, std::string_view("images/abstractnexus-icon.png")));
	icon = std::make_shared<Plane>(window, scene, glm::vec3(height / 2, -height / 2, 0.5), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), glm::vec2(height, height), *iconTexture);
	addChild(icon);
	//
	float toolOptionsX = height;
	float padding = height / 10;
	// File
	fileString = "File";
	float fileFontSize = height / 2;
	float fileLineHeight = 0;
	auto fileTextSize = font.stringSize(fileString, fileFontSize, fileLineHeight, {0, 0});
	file = std::make_shared<Plane>(window, scene, glm::vec3(toolOptionsX = (toolOptionsX + fileTextSize.x / 2 + padding), -height / 2, 0.5), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), fileTextSize, glm::vec4(0.3, 0.3, 0.3, 1));
	fileID = addChild(file);
	fileTextView = std::make_shared<TextView>(window, scene, glm::vec3(0, 0, 0.5f), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), fileString, fileTextSize / 1.5f, font, fileFontSize);
	fileTextView->addToBVH = false;
	file->addChild(fileTextView);
	fileDropdown = std::make_shared<DropdownMenu>(window, scene, glm::vec3(-file->size.x / 2, -file->size.y / 2, 0.5), glm::vec3(0), glm::vec3(1), glm::vec4(0, 0, 0, 1), font);
	fileDropdown->addOption("New Project", []()
	{

	});
	fileDropdown->addOption("Open Project", []()
	{

	});
	fileDropdown->addOption("Settings", []()
	{

	});
	fileDropdown->addOption("Exit", [&]()
	{
		window.close();
	});
	filePressID = file->addMousePressHandler(0, [&](auto &pressed)
	{
		if (!pressed)
		{
			if (fileDropdownID)
			{
				scene.bvh.removeEntity(scene, *fileDropdown);
				file->removeChild(fileDropdownID);
			}
			else
			{
				fileDropdownID = file->addChild(fileDropdown);
				// scene.bvh.addEntity(*fileDropdown);
				std::vector<size_t> entityIDs({ID, fileID, fileDropdownID});
				scene.postAddEntity(fileDropdown, entityIDs);
			}
		}
	});
	// Edit
	editString = "Edit";
	float editFontSize = height / 2;
	float editLineHeight = 0;
	auto editTextSize = font.stringSize(editString, editFontSize, editLineHeight, {0, 0});
	edit = std::make_shared<Plane>(window, scene, glm::vec3(toolOptionsX = (toolOptionsX + fileTextSize.x / 2 + editTextSize.x / 2 + padding), -height / 2, 0.5), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), editTextSize, glm::vec4(0.3, 0.3, 0.3, 1));
	addChild(edit);
	editTextView = std::make_shared<TextView>(window, scene, glm::vec3(0, 0, 0.5f), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), editString, editTextSize / 1.5f, font, editFontSize);
	editTextView->addToBVH = false;
	edit->addChild(editTextView);
	// Help
	helpString = "Help";
	float helpFontSize = height / 2;
	float helpLineHeight = 0;
	auto helpTextSize = font.stringSize(helpString, helpFontSize, helpLineHeight, {0, 0});
	help = std::make_shared<Plane>(window, scene, glm::vec3(toolOptionsX = (toolOptionsX + editTextSize.x / 2 + helpTextSize.x / 2 + padding), -height / 2, 0.5), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), helpTextSize, glm::vec4(0.3, 0.3, 0.3, 1));
	addChild(help);
	helpTextView = std::make_shared<TextView>(window, scene, glm::vec3(0, 0, 0.5f), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), helpString, helpTextSize / 1.5f, font, helpFontSize);
	helpTextView->addToBVH = false;
	help->addChild(helpTextView);
	// Toolbar dragging
	dragMousePressID = addMousePressHandler(0, [&](auto &pressed)
	{
		if (pressed && !dragEnabled)
		{
			SetCapture(window.hwnd);
		}
		else if (!pressed && dragEnabled)
		{
			dragOldCoords = {0, 0};
			ReleaseCapture();
		}
		dragEnabled = pressed;
	});
	globalDragMousePressID = window.addMousePressHandler(0, [&](auto &pressed)
	{
		if (!pressed && dragEnabled)
		{
			dragEnabled = pressed;
			dragOldCoords = {0, 0};
			ReleaseCapture();
		}
	});
	dragMouseMoveID = window.addMouseMoveHandler([&](auto coords)
	{
		if (!dragEnabled)
		{
			return;
		}
		if (dragOldCoords.x == 0 && dragOldCoords.y == 0)
		{
			dragOldCoords = coords;
			return;
		}
		auto diff = coords - dragOldCoords;
		window.setXY(window.windowX + diff.x, window.windowY - diff.y);
		dragOldCoords.x = coords.x - diff.x;
		dragOldCoords.y = coords.y - diff.y;
	});
};
Toolbar::~Toolbar()
{
	xButton->removeMousePressHandler(0, xButtonLeftMousePressID);
	xButton->removeMouseHoverHandler(xButtonMouseHoverID);
	maxButton->removeMousePressHandler(0, maxButtonLeftMousePressID);
	maxButton->removeMouseHoverHandler(maxButtonMouseHoverID);
	_Button->removeMousePressHandler(0, _ButtonLeftMousePressID);
	_Button->removeMouseHoverHandler(_ButtonMouseHoverID);
	removeMousePressHandler(0, dragMousePressID);
	auto &glWindow = ((VAO &)*this).window;
	glWindow.removeMousePressHandler(0, globalDragMousePressID);
	removeMouseMoveHandler(dragMouseMoveID);
}
void Toolbar::preRender()
{
	const auto &model = getModelMatrix();
	shader.bind();
	scene.entityPreRender(*this);
	shader.setBlock("Model", model);
	shader.setBlock("View", scene.view.matrix);
	shader.setBlock("Projection", scene.projection.matrix);
	shader.setBlock("CameraPosition", scene.view.position, 16);
	shader.unbind();
};
void Toolbar::setSize(const glm::vec2& newSize)
{
	height = newSize.y;
	positions = {
		{ 0, -height, 0}, { newSize.x, -height, 0}, { newSize.x, 0, 0 }, { 0, 0, 0} // Front
	};
	updateElements("Position", positions);
	xButton->setSize({ height, height });
	maxButton->setSize({ height, height });
	_Button->setSize({ height, height });
	float toolButtonsX = newSize.x, toolButtonsY = -height / 2;
	xButton->position = { toolButtonsX = (toolButtonsX - (height / 2)), toolButtonsY, 0.5};
	maxButton->position = { toolButtonsX = (toolButtonsX - xButton->size.x), toolButtonsY, 0.5};
	_Button->position = { toolButtonsX = (toolButtonsX - maxButton->size.x), toolButtonsY, 0.5};
	float xFontSize = height;
	float xLineHeight = 0;
	auto xTextSize = font.stringSize(xString, xFontSize, xLineHeight, {0, 0});
	xTextView->setSize(xTextSize / 2.f);
	float maxFontSize = height;
	float maxLineHeight = 0;
	auto maxTextSize = font.stringSize(maxString, maxFontSize, maxLineHeight, {0, 0});
	maxTextView->setSize(maxTextSize / 2.f);
	float _FontSize = height;
	float _LineHeight = 0;
	auto _TextSize = font.stringSize(_String, _FontSize, _LineHeight, {0, 0});
	_TextView->setSize(_TextSize / 2.f);
	icon->position = {height / 2, -height / 2, 0.5};
	icon->setSize({height, height});
	float toolOptionsX = height;
	float padding = height / 10;
	float fileFontSize = height / 2;
	float fileLineHeight = 0;
	auto fileTextSize = font.stringSize(fileString, fileFontSize, fileLineHeight, {0, 0});
	file->position = {toolOptionsX = (toolOptionsX + fileTextSize.x / 2 + padding), -height / 2, 0.5};
	file->setSize(fileTextSize);
	fileTextView->setSize(fileTextSize / 1.5f);
	float editFontSize = height / 2;
	float editLineHeight = 0;
	auto editTextSize = font.stringSize(editString, editFontSize, editLineHeight, {0, 0});
	edit->position = {toolOptionsX = (toolOptionsX + fileTextSize.x / 2 + editTextSize.x / 2 + padding), -height / 2, 0.5};
	edit->setSize(editTextSize);
	editTextView->setSize(editTextSize / 1.5f);
	float helpFontSize = height / 2;
	float helpLineHeight = 0;
	auto helpTextSize = font.stringSize(helpString, helpFontSize, helpLineHeight, {0, 0});
	help->position = {toolOptionsX = (toolOptionsX + editTextSize.x / 2 + helpTextSize.x / 2 + padding), -height / 2, 0.5};
	help->setSize(helpTextSize);
	helpTextView->setSize(helpTextSize / 1.5f);
	scene.bvh.updateEntity(*xButton);
	scene.bvh.updateEntity(*maxButton);
	scene.bvh.updateEntity(*_Button);
	scene.bvh.updateEntity(*this);
	scene.bvh.updateEntity(*file);
	scene.bvh.updateEntity(*edit);
	scene.bvh.updateEntity(*help);
	scene.bvh.updateEntity(*icon);
};
