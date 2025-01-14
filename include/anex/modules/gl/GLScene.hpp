#pragma once
#include <anex/IScene.hpp>
#include "./lights/Lights.hpp"
#include "./lights/PointLight.hpp"
#include "./lights/DirectionalLight.hpp"
#include "./lights/SpotLight.hpp"
#include "./vp/View.hpp"
#include "./vp/Projection.hpp"
#include "./textures/Framebuffer.hpp"
#include "./raytracing/BVH.hpp"
namespace anex::modules::gl
{
	struct GLScene : anex::IScene
	{
		vp::View view;
		vp::Projection projection;
		std::vector<lights::PointLight> pointLights;
		std::vector<lights::DirectionalLight> directionalLights;
		std::vector<lights::SpotLight> spotLights;
		std::vector<lights::PointLightShadow> pointLightShadows;
		std::vector<lights::DirectionalLightShadow> directionalLightShadows;
		std::vector<lights::SpotLightShadow> spotLightShadows;
		glm::vec4 clearColor = glm::vec4(0);
		std::unordered_map<size_t, std::vector<size_t>> triangleIDsToEntityIDsMap;
		textures::Framebuffer *framebufferPointer = 0;
		raytracing::BVH bvh;
		std::array<IWindow::EventIdentifier, 7 - 0 + 1> mousePressIDs;
		IWindow::EventIdentifier mouseMoveID;
		GLEntity *currentHoveredEntity = 0;
    GLScene(IWindow &window, const glm::vec3 &cameraPosition, const glm::vec3 &cameraDirection, const float &fov, textures::Framebuffer *framebufferPointer = 0);
    GLScene(IWindow &window, const glm::vec3 &cameraPosition, const glm::vec3 &cameraDirection, const glm::vec2 &orthoSize, textures::Framebuffer *framebufferPointer = 0);
		~GLScene();
    void preRender() override;
		void render() override;
		void entityPreRender(IEntity &entity) override;
		void resize(const glm::vec2 &newSize) override;
		void postAddEntity(const std::shared_ptr<IEntity> &entity, const std::vector<size_t> &entityIDs) override;
		void preRemoveEntity(const std::shared_ptr<IEntity> &entity, const std::vector<size_t> &entityIDs) override;
		void adjustTriangleIDs(const size_t &minTriangleID, const size_t &triangleIDsSize);
		void adjustTriangleIDsForEntity(GLEntity &entity, const size_t &minTriangleID, const size_t &triangleIDsSize);
		GLEntity *findEntityByPrimID(const size_t &primID);
		void hookMouseEvents();
		void unhookMouseEvents();
  };
}