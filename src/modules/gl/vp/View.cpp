#include <anex/modules/gl/vp/View.hpp>
using namespace anex::modules::gl::vp;
View::View(const glm::vec3& position, const glm::vec3& direction):
	position(position),
	direction(direction),
	phi(atan2(direction.z, direction.x)),
	theta(acos(direction.y))
{
  update();
};
void View::update()
{
	matrix = glm::lookAt(position, position + direction, glm::vec3{0, 1, 0});
};
void View::addPhiTheta(const float &addPhi, const float &addTheta)
{
	phi += addPhi;
	theta += addTheta;
	theta = glm::clamp(theta, 0.001f, glm::pi<float>() - 0.001f);
	glm::dvec3 newDirection;
	newDirection.x = sin(theta) * cos(phi);
	newDirection.y = cos(theta);
	newDirection.z = sin(theta) * sin(phi);
	direction = newDirection + 1e-6;
  update();
};
anex::IWindow::EventIdentifier View::addResizeHandler(const ViewResizeHandler &callback)
{
	auto id = ++viewResizeHandlers.first;
	viewResizeHandlers.second[id] = callback;
	return id;
};
void View::removeResizeHandler(anex::IWindow::EventIdentifier &id)
{
	auto &handlers = viewResizeHandlers.second;
	auto handlerIter = handlers.find(id);
	if (handlerIter == handlers.end())
	{
		return;
	}
	handlers.erase(handlerIter);
	id = 0;
};
void View::callResizeHandler(const glm::vec2 &newSize)
{
	auto& handlersMap = viewResizeHandlers.second;
	std::vector<ViewResizeHandler> handlersCopy;
	for (const auto& pair : handlersMap)
		handlersCopy.push_back(pair.second);
	for (auto& handler : handlersCopy)
	{
		handler(newSize);
	}
};