#include "PlayerMovement.h"
#include "Scene/GraphNode.h"
#include "Scene/CollisionSystem.h"
#include "Scene/Components/Camera.h"
#include <iostream>
#include "Scene/Scenes/TestScene.h"
#include "Scene/Scenes/Scene.h"
#include "glm/glm.hpp"
#include "Serialization/Serializer.h"
#include "Scene/Scenes/EditorScene.h"

float PlayerMovement::mouseSensitivity = 4.0f;

void PlayerMovement::renderGui() {
	Component::renderGui();
	if(active) {
		ImGui::Checkbox("Allow flight", &fly);
		ImGui::Text("Camera source: ");
		ImGui::SameLine();
		if(camera == nullptr) {
			ImGui::Text("null");
		} else if(camera->getGameObject()==gameObject) {
			ImGui::Text("current node");
		} else {
			ImGui::Text(camera->getGameObject()->getName().c_str());
		}
		EditorScene *editor = gameManager->getEditorScene();
		if(editor != nullptr) {
			if(editor->nodeSelectionCallback == nullptr && ImGui::Button("Choose camera")) {
				editor->nodeSelectionCallback = [this](GraphNode* node) {
					setCamera(node->getComponent<Camera>());
				};
			}
		}
	}
}

void PlayerMovement::setFly(bool fly) {
	this->fly = fly;
}

bool PlayerMovement::getFly() {
	return fly;
}

void PlayerMovement::setGravity(bool gravity) {
	this->gravity = gravity;
}

bool PlayerMovement::getGravity() {
	return gravity;
}

Camera* PlayerMovement::getCamera() {
	return camera;
}

void PlayerMovement::setCamera(Camera* camera) {
	this->camera = camera;
	refreshMousePosition = true;
}

SerializableType PlayerMovement::getSerializableType() {
	return SPlayerMovement;
}

Json::Value PlayerMovement::serialize(Serializer* serializer) {
	Json::Value root = Component::serialize(serializer);
	root["camera"] = serializer->serialize(camera);
	root["gravity"] = gravity;
	root["fly"] = fly;
	return root;
}

void PlayerMovement::deserialize(Json::Value& root, Serializer* serializer) {
	Component::deserialize(root, serializer);
	camera = dynamic_cast<Camera*>(serializer->deserialize(root["camera"]).object);
	gravity = root.get("gravity", gravity).asBool();
	fly = root.get("fly", fly).asBool();
}

PlayerMovement::PlayerMovement(GraphNode* _gameObject, Camera *camera) : Component(_gameObject, "Player movement") {
	//if (camera == nullptr) {
	//	throw std::exception("Null camera provided!");
	//}
	if(camera == nullptr) {
		camera = new Camera(gameObject);
		gameObject->addComponent(camera);
	}
	this->camera = camera;
}

PlayerMovement::PlayerMovement(GraphNode* _gameObject) : PlayerMovement(_gameObject, _gameObject->getComponent<Camera>()) {}

void PlayerMovement::update(float msec) {
	if (camera != nullptr && gameManager->getCursorLocked() && gameManager->getCurrentCamera() == camera) {
		if(refreshMousePosition) {
			refreshMousePosition = false;
			lastMousePosition = gameManager->getMousePosition();
		} else {
			glm::vec2 mpos = gameManager->getMousePosition();
			glm::vec2 shift = mpos - lastMousePosition;
			lastMousePosition = mpos;
			if(gameManager->getKeyState(KEY_MOUSE_UP)) {
				shift.y -= 10.0f;
			}
			if (gameManager->getKeyState(KEY_MOUSE_DOWN)) {
				shift.y += 10.0f;
			}
			if (gameManager->getKeyState(KEY_MOUSE_LEFT)) {
				shift.x -= 10.0f;
			}
			if (gameManager->getKeyState(KEY_MOUSE_RIGHT)) {
				shift.x += 10.0f;
			}
			shift *= -mouseSensitivity * msec;
			gameObject->localTransform.rotateYDegrees(shift.x);
			gameObject->localTransform.rotateXDegrees(shift.y);
			auto data = gameObject->localTransform.getData();
			bool changed = false;
			static float clamp = glm::radians(89.9f);
			if(data.eulerRotation.x > clamp) {
				data.eulerRotation.x = clamp;
				changed = true;
			} else if(data.eulerRotation.x < -clamp) {
				data.eulerRotation.x = -clamp;
				changed = true;
			}
			if(changed) {
				gameObject->localTransform.setData(data);
			}
		}

		float speed;
		if (gameManager->getKeyState(KEY_FAST)) {
			speed = 4.0f;
		} else {
			speed = 2.0f;
		}

		glm::vec3 direction = glm::vec3(0.0f);
		if (gameManager->getKeyState(KEY_FORWARD)) {
			direction.z -= speed;
		}
		if (gameManager->getKeyState(KEY_BACKWARD)) {
			direction.z += speed;
		}
		if (gameManager->getKeyState(KEY_LEFT)) {
			direction.x -= speed;
		}
		if (gameManager->getKeyState(KEY_RIGHT)) {
			direction.x += speed;
		}
		if (fly) {
			if (gameManager->getKeyState(KEY_UP)) {
				direction.y += speed;
			}
			if (gameManager->getKeyState(KEY_DOWN)) {
				direction.y -= speed;
			}
		}

		gameObject->moveRelative(direction * msec, fly);
		if (gravity) {
			Collider* collider = gameObject->getComponent<Collider>();
			GraphNode* floor = CollisionSystem::getInstance()->castRay(gameObject->worldTransform.getPosition() + glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0, -1, 0), 0.5f, collider);
			if (floor == nullptr) {
				gameObject->localTransform.translate(glm::vec3(0.0f, -2.0f * msec, 0.0f));
			}
		}
	} else {
		refreshMousePosition = true;
	}
}
