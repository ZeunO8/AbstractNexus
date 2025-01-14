#pragma once
#include <anex/IEntity.hpp>
#include <anex/IWindow.hpp>
#include <anex/modules/gl/vaos/VAO.hpp>
#include <anex/modules/gl/shaders/Shader.hpp>
namespace anex::modules::gl
{
	struct GLEntity : anex::IEntity, vaos::VAO
	{
		std::vector<uint32_t> indices;
		std::vector<glm::vec3> positions;
    glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
		glm::mat4 model;
		shaders::Shader &shader;
		bool affectedByShadows = true;
		size_t childrenCount = 0;
		std::map<size_t, std::shared_ptr<GLEntity>> children;
		GLEntity *parentEntity = 0;
		bool addToBVH = true;
		std::unordered_map<IWindow::Button, int> buttons;
		std::unordered_map<IWindow::Button, std::pair<IWindow::EventIdentifier, std::map<IWindow::EventIdentifier, IWindow::MousePressHandler>>> mousePressHandlers;
		std::pair<IWindow::EventIdentifier, std::map<IWindow::EventIdentifier, IWindow::MouseMoveHandler>> mouseMoveHandlers;
		using MouseHoverHandler = std::function<void(const bool &)>;
		std::pair<IWindow::EventIdentifier, std::map<IWindow::EventIdentifier, MouseHoverHandler>> mouseHoverHandlers;
		std::vector<size_t> triangleIDs;
		GLEntity(anex::IWindow &window,
						 const shaders::RuntimeConstants &constants,
						 const uint32_t &indiceCount,
						 const std::vector<uint32_t> &indices,
						 const uint32_t &elementCount,
						 const std::vector<glm::vec3> &positions,
						 const glm::vec3 &position,
						 const glm::vec3 &rotation,
						 const glm::vec3 &scale);
		virtual void update();
		virtual void preRender();
    void render() override;
		virtual void postRender();
		const glm::mat4 &getModelMatrix();
		size_t addChild(const std::shared_ptr<GLEntity> &child);
		void removeChild(size_t &ID);
		IWindow::EventIdentifier addMousePressHandler(const IWindow::Button &button, const IWindow::MousePressHandler &callback);
		void removeMousePressHandler(const IWindow::Button &button, IWindow::EventIdentifier &id);
		IWindow::EventIdentifier addMouseMoveHandler(const IWindow::MouseMoveHandler &callback);
		void removeMouseMoveHandler(IWindow::EventIdentifier &id);
		IWindow::EventIdentifier addMouseHoverHandler(const MouseHoverHandler &callback);
		void removeMouseHoverHandler(IWindow::EventIdentifier &id);
		void callMousePressHandler(const IWindow::Button &button, const int &pressed);
		void callMouseMoveHandler(const glm::vec2 &coords);
		void callMouseHoverHandler(const bool &hovered);
  };
}